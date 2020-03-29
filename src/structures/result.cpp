#include "result.h"
#include "../algorithm/network.h"

#include "operations/operation.h"
#include "operations/indexscan.h"
#include "operations/indexcon.h"
#include "operations/seqscan.h"

#include "../db/program.h"

#include <stdio.h>
#include <queue>
#include <string>
#include <queue>

using namespace std;

char AND_STR[] = "AND";
char NOT_STR[] = "NOT";
char OR_STR[]  = "OR";

Result::Result() {}

Result::Result(Table* table, std::vector<Column*>* columns) {
    this->table = table;
    for(Column* c : *columns) {
        this->columns.push_back(c);
    }
}

void Result::print() {
    printf("u %s\n",  table->getTableName());
    printf(" nad retcima: ");
    for(Column* column : this->columns) {
        printf("%s, ", column->getName());
    }
    printf("\n");
    
}


void Result::setLock() {
    this->locked = true;
}

void Result::setParent(Result* parent) {
    this->parent = parent;
}

void Result::setEqualType(int equalType) {
    this->equaltype = equalType;
}

Table* Result::getTable() {
    return this->table;
}
vector<Column*> Result::getColumns() {
    return this->columns;
}
Result* Result::getParent() {
    return this->parent;
}

int Result::getEqualType() {
    return this->equaltype;
}

bool Result::hasColumn(char* col_name) {
    if(this->getParent()->locked == true) return false;
    for(Column* col : columns) {
        if(strcmp(col->getName(), col_name) == 0) return true;
    }
    return false;
}

float Select::getCost(Database* database) {
    return this->operation->getCost(database);
}

bool Select::compare_index_pointer(pair<Index*, pair<int, int> > a, pair<Index*, pair<int, int> > b) {
    return strcmp(a.first->getTable(), b.first->getTable()) < 0;
}

Select::Select(Database* database, vector<table_name*>* tables, vector<variable>* variables) {
}

Select::Select(Database* database, node* root, vector<table_name*>* tables, vector<variable>* variables) {

    Program* program = Program::getInstance();
    this->database = database;
    //remove not nodes
    root = this->de_morgan(root);
    printf("Root name: %s\n", root->name);

    for(table_name* t : *tables) {
        printf("Table: %s %s\n", t->name, t->real_name);
    }
    //asign tables to the variables
    for(int i = 0, size = variables->size(); i < size; ++i) {

        variable* v = &(*variables)[i];

        if(v->table[0] == 0) {
            strcpy(v->table, database->getTableNameByVar(v->name, tables));
        } else {
            for(table_name* t : *tables) {
                if(strcmp(t->name, v->table) == 0) {
                    v->setTable(t->real_name);
                }
            }
        }
    }

    this->or_node = 0;
    this->table_count = 0;
    dfs(root);
    table_count = tables_set.size();

    printf("Tc: %d or_node: %d\n", table_count, or_node);

    if(this->or_node > 0 && this->table_count > 1) {
        //seq scan na sve
        Operation *parent = operation;
        for(table_name* t : *tables) {
            Table* table = database->getTable(t->real_name);
            Operation *_op = new SeqScan(table);
            parent->addChild(_op);
            parent = _op;
        }
    } else {
        
        vector<vector<expression_info*> > areas = this->getAreas(root);
        
        printf("Areas num %d %d\n", areas.size(), areas[0].size());
        
        if(this->or_node > 0 && this->table_count == 1) {
           //pretraga indeksa po područjima, jedna tablica
           
           vector<string> indexed_tables;
            for(Table _table : tables_set) {
                for(auto area : areas) {
                    //construct  network
                    Table* table = database->getTable(_table.getTableName());

                    Network* network = new Network(database, table, table->getIndex(), area, indexed_tables);
                    
                    for(pair<Index*, pair<int, int> > pair : network->getUsedIndexes()) {
                        int isScan = pair.second.first;
                        int len  = pair.second.second;
                        Operation *op;

                        if(!isScan) {
                            op = new IndexCon(table, pair.first, len, network->getRetrData());
                        } else {
                            op = new IndexScan(table, pair.first, len, network->getRetrData());
                        }
                        operation->addChild(op);
                    }

                    if(network->getUsedIndexes().size() == 0) {
                        Operation *op = new SeqScan(table);
                        operation->addChild(op);
                    }   
                }
            }


        } else if(this->or_node == 0) {
            //jedno područje and-a

            Operation *parent = operation;

            //seq scan nad onima koji se ne spominju
            for(table_name* table_name : *tables) {
                Table _table = *(database->getTable(table_name->real_name));
                printf("table_name %s %s\n", table_name->name, table_name->real_name);
                auto i1 = tables_set.find(_table);
                if(i1 == tables_set.end()) {
                    Operation *op = new SeqScan(&_table);
                    parent->addChild(op);
                    parent = op;
                }
            }


            vector<expression_info*> area = areas[0];
            map<Table, bool> seq_scan_tables;
            vector<string> indexed_tables;

            vector<pair<Index*, pair<int, int> >> used_indexes; 
            
            map<Table, bool> retr_data_tables;
            
            printf("OrNode %d Table_set %d\n", or_node, tables_set.size());
            for(Table _table : tables_set) {
                if(seq_scan_tables[_table]) continue;
                //construct  network
                
                Table* table = database->getTable(_table.getTableName());
                printf("table json: %s\n", table->getTableName());
                printf("%d %d\n", table->getIndex().size(), table);
                Network* network = new Network(database, table, table->getIndex(), area, indexed_tables);
                
                if(network->getRetrData()) retr_data_tables[_table] = true;

                if(network->getUsedIndexes().size() > 0) {
                    indexed_tables.push_back(string(table->getTableName()));
                    for(auto in : network->getUsedIndexes()) {
                        used_indexes.push_back(in);
                    }
                }

                for(Table _t : network->getSeqScan()) {
                    seq_scan_tables[_t] = true;
                }
            }
            Operation *first = operation;
            for(auto it = seq_scan_tables.begin(); it != seq_scan_tables.end(); ++it) {
                if(seq_scan_tables[it->first] == true) {
                    Table* t = (Table*)&(it->first);
                    Operation *op = new SeqScan(t);
                    parent->addChild(op);
                    parent = op;
                }
            }
            printf("Size %d\n", used_indexes.size());
            //sort(used_indexes.begin(), used_indexes.end(), Select::compare_index_pointer);
            char *p = 0;
            
            

            for(auto _index : used_indexes) {
                Index* index = _index.first;
                int isScan = _index.second.first;
                int len = _index.second.second;
                Table* table = database->getTable(index->getTable());
                
                printf("Table name: %s\n", index->getTable());
                printf("Table name: %s\n", table->getTableName());
                Operation *op;
                bool retr_data = retr_data_tables[*table];
                if(isScan) {   
                    op = new IndexScan(table, index, len, retr_data);
                } else {
                    op = new IndexCon(table, index, len, retr_data);
                }

                if(p == 0 || p != index->getTable()) {
                    Operation *table_in = new Operation(0);
                    parent->addChild(table_in);

                    parent = table_in;
                    parent->addChild(op);
                    p = index->getTable();
                } else {
                    parent->addChild(op);
                }
            }
            printf("===============\n");
            first->print();
            printf("===============\n");
        }
    }
}

vector<vector<expression_info*> > Select::getAreas(node *root) {
    vector<vector<expression_info*>> res;
    if(root->terminal) {
        vector<expression_info*> v1;
        v1.push_back(root->e1);
        res.push_back(v1);
    } else {
        auto left = getAreas(root->left);
        auto right = getAreas(root->right);

        int size_left = left.size();
        int size_right = right.size();
        
        for(int i = 0; i < size_left - 1; ++i) {
            res.push_back(left[i]);
        }
        for(int i = 0; i < size_right - 1; ++i) {
            res.push_back(right[i]);
        }

        if(strcmp(root->name, AND_STR) == 0) {
            //AND
            vector<expression_info*>v1;
            v1.insert(v1.end(), left[size_left-1].begin(), left[size_left-1].end());
            v1.insert(v1.end(), right[size_right-1].begin(), right[size_right-1].end());
            res.push_back(v1);
        } else if(strcmp(root->name, OR_STR) == 0) {
            //OR
            if(left[size_left-1].size() != 0) {
                res.push_back(left[size_left-1]);

                vector<expression_info*>v1;
                left.push_back(v1);
                ++size_left;
                
            } else if(right[size_right-1].size() != 0) {
                res.push_back(right[size_right-1]);

                vector<expression_info*>v1;
                right.push_back(v1);
                ++size_right;
            }

            vector<expression_info*>v2;
            v2.insert(v2.end(), left[size_left-1].begin(), left[size_left-1].end());
            v2.insert(v2.end(), right[size_right-1].begin(), right[size_right-1].end());
            
            res.push_back(v2);
        }
    }
    return res;
}

void Select::dfs(node *root) {
    
    if(!root->terminal) {
        
        if(strcmp(root->name, AND_STR) == 0) { // and
            dfs(root->left);
            dfs(root->right);    
        } else { // or
            dfs(root->left);
            dfs(root->right);    
            ++or_node;
        }
    } else { //terminal root

        vector<variable> variables = *root->e1->variables;
        for(auto v : variables) {
            Table table = *database->getTable(v.table);
            tables_set.insert(table);
        }
    }
    
}


node* Select::de_morgan(node* root) {
    
    if(root->terminal) {
        return root;
    }
    
    if(strcmp(root->name, NOT_STR) == 0) {
        if(!root->left->terminal && strcmp(root->left->name, NOT_STR) != 0){
            node* n = new node();
            node* not1 = new node();
            node* not2 = new node();
            
            n->left = not1;
            n->right = not2;
            
            //set name
            if(strcmp(root->left->name, OR_STR) == 0) {
                memset(n->name, 0, 4);
                strcpy(n->name, AND_STR);
            } else {
                memset(n->name, 0, 4);
                strcpy(n->name, OR_STR);
            }
            strcpy(not1->name, NOT_STR);
            strcpy(not2->name, NOT_STR);
            
            //set children
            not1->left = root->left->left;
            not2->left = root->left->right;
            
            n->left = this->de_morgan(n->left);
            
            n->right = this->de_morgan(n->right);
            
            return n;
        } else if(!root->left->terminal && strcmp(root->left->name, NOT_STR) == 0) {
            //skip one not
            return de_morgan(root->left->left);
            
        } else if(root->left->terminal) {
            if(root->left->e1->equal != 0) {
                root->left->e1->equal = 3 - root->left->e1->equal;
            }
            return root->left;
        }
    } else {
        node* n1 = this->de_morgan(root->left);
        if(root->left != n1) {
            root->left = n1;
        }
        node* n2 = this->de_morgan(root->right);
        if(root->right != n2) {
            root->right = n2;
        }
    }
    return root;
}
