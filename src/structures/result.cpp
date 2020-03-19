#include "result.h"
#include <stdio.h>
#include <queue>
#include <set>
#include <string>
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
    this->type = SEQ_SCAN;
    this->locked = false;
}

void Result::removeAndFlood() {
    this->and_flood = false;
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

Table* Result::getTable() {
    return this->table;
}
vector<Column*> Result::getColumns() {
    return this->columns;
}
Result* Result::getParent() {
    return this->parent;
}

bool Result::hasColumn(char* col_name) {
    for(Column* col : columns) {
        if(strcmp(col->getName(), col_name) == 0) return true;
    }
    return false;
}


Select::Select(Database* database, vector<table_name*>* tables, vector<variable>* variables) {
}

Select::Select(Database* database, node* root, vector<table_name*>* tables, vector<variable>* variables) {

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

    vector<Result*> p = dfs(root);
    for(auto r : p) {
        r->print();
    }
    
}

void Select::process(vector<Result*> inputs) {
    //try to find index
    set<string> tables;
    for(Result* _r : inputs) {
        tables.insert(string(_r->getTable()->getTableName()));
    }
    for(string table_name : tables) {
        Table* _table = database->getTable(table_name.c_str());
        vector<Index*> indexes = _table->getIndex();
        if(indexes.size() == 0) continue;

        while(true) {
            Index* best_index = indexes[0];
            int mini_len = MAX_INTEGER;
            for(Index* _index : indexes) {
                int len = this->lookForIndex(_index, inputs).size();
                if(len < mini_len) {
                    mini_len = len;
                    best_index = _index;
                }
            }
            if(mini_len == inputs.size()) break;
            inputs = this->createIndexScan(best_index, inputs);
        }
    }
}

vector<Result*> Select::dfs(node *root, int depth) {
    
    vector<Result*> results;
    if(!root->terminal) {
        
        if(strcmp(root->name, AND_STR) == 0) { // and
            auto r1 = dfs(root->left, depth + 1);
            auto r2 = dfs(root->right, depth + 1);    

            results.insert(results.end(), r1.begin(), r2.end());
            results.insert(results.end(), r2.begin(), r2.end());

            if(depth == 0) process(results);
        } else { // or
            auto r1 = dfs(root->left, depth + 1);
            auto r2 = dfs(root->right, depth + 1);    

            process(r1);
            process(r2);
        }
        return results;
        
    } else { //terminal root

        vector<Result*> list;
        vector<variable> variables = *root->e1->variables;
        set<string> tables;
        for(auto v : variables) {
            tables.insert(string(v.table));
        }
        int size = tables.size();
        Result* parent = new Result();
        if(root->e1->locked) {
            parent->setLock();
        }

        for(string t : tables) {
            Table* table = database->getTable(t.c_str());
            vector<Column*>* columns = new vector<Column*>();
            for(auto v: variables) {
                if(strcmp(table->getTableName(), v.table) == 0) {
                    columns->push_back(table->getColumn(v.name));
                }
            }
            Result* _r = new Result(table, columns);
            _r->setParent(parent);    
            list.push_back(_r);
        }
        
        return list;
    }
    
}

vector<Result*> Select::createIndexScan(Index* index, vector<Result*> inputs) {
    vector<Result*> output = this->lookForIndex(index, inputs);
    //add this index into list of actions
    return output;
}

vector<Result*> Select::lookForIndex(Index* index, vector<Result*> inputs) {
    vector<Result*> left = inputs;

    for(int col_id = 0; col_id < index->getColNumber(); ++col_id) {
        vector<Result*> _left;
        char* col_name = index->getColName(col_id);
        printf("index col %s\n", col_name);
        for(Result* result : left) {
            printf("%d\n", result->hasColumn(col_name));
            if(result->hasColumn(col_name)) {
                _left.push_back(result);
            }
        }
        printf("%d/%d %d Name: %s, col: %s\n", col_id, index->getColNumber(), left.size(), index->getName(), col_name);
        if(_left.size() == 0 && col_id == 0) return inputs;
        left = _left;
    }
    printf("Found index");
    return left;
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
