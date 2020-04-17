#include "operations/filter.h"
#include "operations/group.h"
#include "operations/hash-join.h"
#include "operations/indexcon.h"
#include "operations/indexscan.h"
#include "operations/merge-join.h"
#include "operations/nested-join.h"
#include "operations/operation.h"
#include "operations/or-union.h"
#include "operations/seqscan.h"
#include "operations/sort.h"

#include "../algorithm/network.h"
#include "../algorithm/toposort.h"

#include "database.h"
#include "index.h"
#include "column.h"
#include "result.h"

#include "../db/program.h"

#include <stdio.h>
#include <queue>
#include <vector>
#include <string>
#include <queue>
#include <iostream>

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

double Select::getCost(Database* database) {
    double cost = this->root->getTotalCost(database);
    for(Select* sibling : siblings) {
        if(sibling != 0 ) cost += sibling->getCost(database);
    }
    printf("init cost %lf\n", cost);
    double nt = this->root->getNt();
    for(Select* kid : kids) {
        if(kid == 0) continue;
        double select_cost = kid->getCost(database);
        printf("cost in subquery %lf * %lf\n", select_cost,nt);
        printf("precost %lf\n", cost);
        cost += (nt * select_cost);
        printf("fin cost %lf\n", cost);
    }

    printf("Res: %lf, NT: %lf\n", cost, nt);
    return cost;
}

double Select::getLoadingCost(Database* database) {
    return 0;
}

double Select::getFinalCost(Database* database) {
    return this->getCost(database) + this->getLoadingCost(database);
}

resursi Select::getResource() {
    for(Select* sibling : siblings) {
        this->resouce = Select::mergeResource(sibling->getResource(), resouce);
    }
    for(Select* kid : kids) {
        this->resouce = Select::mergeResource(kid->getResource(), resouce);
    }
    return resouce;
}

bool Select::compare_index_pointer(pair<Index*, pair<int, int> > a, pair<Index*, pair<int, int> > b) {
    return strcmp(a.first->getTable(), b.first->getTable()) < 0;
}

Select::Select() {}

Select::Select(Database* database, node* root, vector<table_name*>* tables, vector<variable>* variables) {

    Program* program = Program::getInstance();
    this->database = database;
    //remove not nodes
    
    root = this->de_morgan(root);
    
    
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


    

    if((this->or_node > 0 && this->table_count > 1) || root == 0) {
        //seq scan na sve
        int numOfOperations = sumOperations(root);

        vector<SeqScan*> seqscans;
        for(table_name* t : *tables) {
            Table* table = database->getTable(t->real_name);
            SeqScan *_op = new SeqScan(table);
            seqscans.push_back(_op);
        }
        Operation* op =(Operation*) new NestedJoin();
        op->addChild(seqscans[0]);
        op->addChild(new Dummy());

        vector<Table*> tables_in_join;
        tables_in_join.push_back(seqscans[0]->getTable());
        
        vector<expression_info*> area;

        auto list = getAreas(root);
        for(auto sublist : list) {
            area.insert(area.end(), sublist.begin(), sublist.end());
        }

        for(int i = 1, len = seqscans.size(); i < len; ++i) {
            Operation *join = new NestedJoin();
            join->addChild(op);
            join->addChild(seqscans[i]);

            vector<pair<bool, int> > filter_list = numOfFilter(tables_in_join, area);
            Operation *node = join;
            for(int i = 0, len = filter_list.size(); i < len; ++i) {
                Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                filter->addChild(node);
                node = filter;
            }

            op = node;
        }

        this->root = op;

    } else {
        
        vector<vector<expression_info*> > areas = this->getAreas(root);
        map<string, Operation*> tables_operations;
        if(this->or_node >= 0 && this->table_count == 1) {
            printf("Jedna tablica\n");
           //pretraga indeksa po područjima, jedna tablica

            bool found = false;
            vector<string> indexed_tables;
            vector<pair<Operation*, pair<int, bool> > > operations;
            int totalNumOfOperations = 0;
            for(Table _table : tables_set) {
                for(auto area : areas) {
                    //construct  network
                    
                    Table* table = database->getTable(_table.getTableName());
                    int numOfOp = 0;
                    for(expression_info* exp : area) {
                        numOfOp += exp->oper;
                    }
                    totalNumOfOperations += numOfOp;
                    TopoSort *t = new TopoSort(database, new Dummy());
                    Network* network = new Network(database, table, table->getIndex(), area, &tables_operations);
                    if(network->getUsedIndexes().size() == 0) {
                        break;
                    }
                    for(pair<Index*, pair<int, int> > pair : network->getUsedIndexes()) {
                        int isScan = pair.second.first;
                        int len  = pair.second.second;
                        bool retr_data = network->getRetrData();
                        Operation *op;

                        if(!isScan) {
                            operations.push_back(make_pair(new IndexCon(table, pair.first, len, retr_data), make_pair(numOfOp, retr_data)));
                        } else {
                            operations.push_back(make_pair(new IndexScan(table, pair.first, len, retr_data), make_pair(numOfOp, retr_data)));
                        }
                        
                    }

                    if(network->getUsedIndexes().size() == 0) {
                        operations.clear();
                    }   
                }
            }
            if(operations.size() == 0) { // nisam našao indekse
                //seq scan i filter 
                Table* table = database->getTable(((Table)*tables_set.begin()).getTableName());
                
                SeqScan* scan = new SeqScan(table);
                if(totalNumOfOperations > 0) { //nema potrebe za filterom
                    Filter* filter = new Filter(true, totalNumOfOperations);
                    filter->addChild(scan);
                    this->root = filter;
                } else {
                    this->root = scan;
                }
            } else { // radim višestruke indeks scanove sa union operatorom i mogucim filterom
                Table* table = database->getTable(((Table)*tables_set.begin()).getTableName());

                OrUnion *orUnion = new OrUnion();

                for(auto par : operations) {
                    auto operation = par.first;
                    int numOfOp   = par.second.first;
                    bool retr_data = par.second.second;
                    if(retr_data) {
                        Filter* filter = new Filter(true, numOfOp);
                        filter->addChild(operation);
                        orUnion->addChild(filter);
                    } else {
                        orUnion->addChild(operation);
                    }
                }
                this->root = orUnion;
            }

        } else if(this->or_node == 0) {
            //jedno područje and-a

            vector<expression_info*> area = areas[0];

            //seq scan nad onima koji se ne spominju, izgradnja nestedJoin-a nad njima
            vector<Operation*> seqscans;
            for(table_name* table_name : *tables) {
                Table _table = *(database->getTable(table_name->real_name));
                string table_name_str = string(_table.getTableName());

                tables_operations[string(table_name->real_name)] = 0;

                auto i1 = tables_set.find(_table);
                if(i1 == tables_set.end()) { //tablica nije referirana u uvjetima
                    
                    Operation* scan = new SeqScan(&_table);
                    vector<Table*> table_list; table_list.push_back(&_table);
                    vector<pair< bool, int>> filter_list = numOfFilter(table_list, area);

                    for(int i = 0, len = filter_list.size(); i < len; ++i) {
                        Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                        filter->addChild(scan);
                        scan = filter;
                    }
                    tables_operations[table_name_str] = scan;
                    seqscans.push_back(scan);
                }
            }
            Operation *parent = 0;
            if(seqscans.size() > 0) {
                parent = seqscans[0];
                
                for(int i = 1, len = seqscans.size(); i < len; ++i) {
                    NestedJoin* join = new NestedJoin();
                    join->addChild(parent);
                    join->addChild(seqscans[i]);
                    setNewOperation(parent, join, &tables_operations);
                    setNewOperation(seqscans[i], join, &tables_operations);
                    parent = join;    
                }
            } 
            
            
            for(table_name* table_name : *tables) {
                Table *table = database->getTable(table_name->real_name);
                string str_table_name = string(table_name->real_name);
                vector<Operation*> found_indexes;
                
                for(Index* index : table->getIndex()) {
                    bool index_found = true;
                    int col_id = 0;
                    for(int col_num = index->getColNumber(); col_id < col_num; ++col_id) {
                        bool found = false;
                        for(expression_info* exp : area) {
                            if(exp->equal != 1) continue;
                            if(exp->variables->size() != 1) continue;

                            variable variable = (*(exp->variables))[0];
                            if(strcmp(variable.name, index->getColName(col_id)) == 0) found = true;
                        }
                        if(!found  && col_id > 0) {
                            index_found = true;
                            break;
                        } else if(!found && col_id == 0) {
                            index_found = false;
                            break;
                        }
                    }
                    if(index_found) {
                        found_indexes.push_back(new IndexCon(table, index, col_id, true));
                    }
                }
                for(Operation* index_con : found_indexes) {
                    if(tables_operations[str_table_name] != 0) {
                        OrUnion* orUnion = new OrUnion();
                        orUnion->addChild(index_con);
                        orUnion->addChild(tables_operations[str_table_name]);
                        tables_operations[str_table_name] = orUnion;
                    } else {
                        tables_operations[str_table_name] = index_con;
                    }
                }
            }





            if(parent == 0) parent = new Dummy(); //set initial parent node
            
            set<string> tables_candidates_foreign;
            for(expression_info* exp : area) {
                if(exp->variables->size() == 2 && exp->equal == 1) {
                    string table1 = string((*(exp->variables))[0].table);
                    string table2 = string((*(exp->variables))[1].table);
                    if(table1.compare(table2) != 0) { // imamo situaciju a.x = b.y
                        
                        tables_candidates_foreign.insert(table1);
                        tables_candidates_foreign.insert(table2);
                    }
                }
            }

            for(string table1 : tables_candidates_foreign) {
                for(string table2 : tables_candidates_foreign) {
                    if(table1.compare(table2) == 0) continue;
                    vector<char*> col1;
                    vector<char*> col2;

                    for(expression_info* exp : area) {
                        if(exp->variables->size() == 2) {
                            string exp_t1 = string((*(exp->variables))[0].table);
                            string exp_t2 = string((*(exp->variables))[1].table);
                            if(table1.compare(exp_t1) == 0 && table2.compare(exp_t2) == 0) {
                                col1.push_back((*(exp->variables))[0].name);
                                col2.push_back((*(exp->variables))[1].name);
                            } else if(table1.compare(exp_t2) == 0 && table2.compare(exp_t1) == 0) {
                                col1.push_back((*(exp->variables))[1].name);
                                col2.push_back((*(exp->variables))[0].name);
                            }
                        }
                    }
                    
                    if(col1.size() > 0 && 
                    database->isForeignKey(table1.c_str(), col1, table2.c_str(), col2)) { //našao foreign key

                        if(tables_operations[table2] == 0) { //desna tablica nije u join-u
                            HashJoin* hashJoin = new HashJoin(true);
                            
                            if(tables_operations[table1] == 0) {
                                Table* table = database->getTable((char*) table1.c_str());
                                SeqScan* seqScan = new SeqScan(table);
                                
                                vector<pair<bool, int>> filter_list = numOfFilter(Select::getTables(database, tables_operations, tables_operations[table1]), area);
                                Operation *node = seqScan;
                                for(int i = 0, len = filter_list.size(); i < len; ++i) {
                                    Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                                    filter->addChild(node);
                                    node = filter;
                                }
                                tables_operations[table1] = node;

                            } 
                            if(tables_operations[table2] == 0) {
                                //init
                                Table* table = database->getTable((char*) table2.c_str());
                                
                                Index* index = table->getIndexByCol(col2);
                                
                                IndexScan* indexScan = new IndexScan(table, index, 1, 0);
                                vector<Table*> table_list; table_list.push_back(table);
                                vector<pair<bool, int> > filter_list = numOfFilter(table_list, area);
                                Operation *node = indexScan;
                                for(int i = 0, len = filter_list.size(); i < len; ++i) {
                                    Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                                    filter->addChild(node);
                                    node = filter;
                                }
                                tables_operations[table2] = node;

                            }
                            hashJoin->addChild(tables_operations[table1]);
                            hashJoin->addChild(tables_operations[table2]);

                            setNewOperation(tables_operations[table1], hashJoin, &tables_operations);
                            setNewOperation(tables_operations[table2], hashJoin, &tables_operations);
                        }
                    }


                }
            }


            for(Table _table : tables_set) {
                string str_table_name = string(_table.getTableName());
                if(tables_operations[str_table_name] != 0) continue;
                
                Table* table = database->getTable(_table.getTableName());
                
                Network* network = new Network(database, table, table->getIndex(), area, &tables_operations);
                
                for(Table t : network->getSeqScan()) {
                    SeqScan* scan = new SeqScan(&t);
                    string table_name = string(t.getTableName());
                    vector<Table*> table_list; table_list.push_back(&t);
                    
                    vector<pair<bool ,int> > filter_list = numOfFilter(table_list, area);
                    Operation *node = scan;
                    for(int i = 0, len = filter_list.size(); i < len; ++i) {
                        Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                        filter->addChild(node);
                        node = filter;
                    }
                    tables_operations[table_name] = node;
                }

                for( auto in : network->getUsedIndexes()) {
                    Index* index = in.first;
                    int isScan = in.second.first;
                    int len = in.second.second;

                    Operation *index_search;
                    if(isScan) index_search = new IndexScan(table, index, len, network->getRetrData());
                    else       index_search = new IndexCon(table, index, len, network->getRetrData());

                    if(tables_operations[str_table_name] != 0) {
                        OrUnion* orUnion = new OrUnion();
                        orUnion->addChild(tables_operations[str_table_name]);
                        orUnion->addChild(index_search);
                        tables_operations[str_table_name] = orUnion;
                    } else {
                        tables_operations[str_table_name] = index_search;
                    }

                }
            }

            for(table_name* table_name : *tables) {
                string str_table_name = string(table_name->real_name);
                if(tables_operations[str_table_name] == 0) {
                    Table* table = database->getTable((char*) str_table_name.c_str());
                    SeqScan* seqScan = new SeqScan(table);
                    
                    vector<pair<bool, int> > filter_list = numOfFilter(Select::getTables(database, tables_operations, tables_operations[str_table_name]), area);
                    Operation *node = seqScan;
                    for(int i = 0, len = filter_list.size(); i < len; ++i) {
                        Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                        filter->addChild(node);
                        node = filter;
                    }
                    tables_operations[str_table_name] = node;
                }
            }


            while(true) { //postavi jedinstveni join nad svima
                string first_table = string((*tables)[0]->real_name);
                Operation* pointer = tables_operations[first_table]; 
                bool done = true;
                for(table_name* table_name : *tables) {
                    string str_table_name = string(table_name->real_name);
                    if(tables_operations[str_table_name] != pointer) {
                        NestedJoin* join = new NestedJoin();
                        join->addChild(pointer);
                        join->addChild(tables_operations[str_table_name]);
                        
                        setNewOperation(tables_operations[str_table_name], join, &tables_operations);
                        setNewOperation(pointer,                           join, &tables_operations);
                        
                        vector<Table*> tables_in_join = Select::getTables(database, tables_operations, join);

                        vector<pair<bool, int> > filter_list = numOfFilter(tables_in_join, area);
                        Operation *node = join;
                        for(int i = 0, len = filter_list.size(); i < len; ++i) {
                            Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                            filter->addChild(node);
                            node = filter;
                        }
                        setNewOperation(tables_operations[str_table_name], node, &tables_operations);
                        setNewOperation(pointer,                           node, &tables_operations);
                        
                    }
                }
                if(done) {
                    string first_table = string((*tables)[0]->real_name);
                    Operation* res_root = tables_operations[first_table]; 
                    this->root = res_root;
                    break;
                }
            }





            /*map<Table, bool> seq_scan_tables;
            vector<string> indexed_tables;

            vector<pair<Index*, pair<int, int> >> used_indexes; 
            
            map<Table, bool> retr_data_tables;
            
            for(Table _table : tables_set) {
                cout << "gledam na tablicu " << _table.getTableName() <<  " " << seq_scan_tables[_table] << endl;
                if(seq_scan_tables[_table]) continue;
                //construct  network
                
                Table* table = database->getTable(_table.getTableName());
                
                Network* network = new Network(database, table, table->getIndex(), area, indexed_tables, toposort);
                
                if(network->getRetrData()) retr_data_tables[_table] = true;
                printf("%s %d\n", _table.getTableName(), network->getUsedIndexes().size());
                if(network->getUsedIndexes().size() > 0) {
                    indexed_tables.push_back(string(table->getTableName()));
                    
                    string table_name = string(_table.getTableName());
                    
                    if(network->getUsedIndexes().size() > 1) {
                        tables_operations[table_name] = new OrUnion();
                        for(auto in : network->getUsedIndexes()) {
                            used_indexes.push_back(in);

                            Index* index = in.first;
                            int isScan = in.second.first;
                            int len = in.second.second;
                            
                            bool retr_data = retr_data_tables[*table];
                            if(isScan) {   
                                tables_operations[table_name]->addChild(new IndexScan(table, index, len, retr_data));
                            } else {
                                tables_operations[table_name]->addChild(new IndexCon(table, index, len, retr_data));
                            }
                        }
                    } else {
                        auto in = network->getUsedIndexes()[0];
                        used_indexes.push_back(in);

                        Index* index = in.first;
                        int isScan = in.second.first;
                        int len = in.second.second;
                        
                        bool retr_data = retr_data_tables[*table];
                        if(isScan) {   
                            tables_operations[table_name] = new IndexScan(table, index, len, retr_data);
                        } else {
                            tables_operations[table_name] = new IndexCon(table, index, len, retr_data);
                        }
                        printf("%d %d %d\n", len, retr_data, index->getColNumber());
                        printf("isScan %d index runtime %f\n", isScan, tables_operations[table_name]->getRuntimeCost(database));
                    }
                } else seq_scan_tables[_table] = true;

                for(Table _t : network->getSeqScan()) {
                    seq_scan_tables[_t] = true;
                }
            }

            
            printf("scan %d\n", seq_scan_tables.size());
            for(auto it = seq_scan_tables.begin(); it != seq_scan_tables.end(); ++it) {
                
                if(seq_scan_tables[it->first] == true) {
                    Table* t = (Table*)&(it->first);
                    string table_name = string(t->getTableName());
                    cout << table_name << endl;
                    SeqScan* scan = new SeqScan(t);
                    tables_operations[table_name] = scan;
                    toposort->addNode(t->getTableName());        
                }
            }
            printf("Size %d\n", used_indexes.size());
            sort(used_indexes.begin(), used_indexes.end(), Select::compare_index_pointer);
            char *p = 0;
            
            
            printf("Used indexes %d\n", used_indexes.size());
            for(auto _index : used_indexes) {
                Index* index = _index.first;
                int isScan = _index.second.first;
                int len = _index.second.second;
                Table* table = database->getTable(index->getTable());
                string str_table_name = string(table->getTableName());
                cout << "index " << str_table_name << endl;
                Operation *op;
                bool retr_data = retr_data_tables[*table];
                if(isScan) {   
                    op = new IndexScan(table, index, len, retr_data);
                } else {
                    op = new IndexCon(table, index, len, retr_data);
                }
                
                if(tables_operations[str_table_name] != 0) {
                    tables_operations[str_table_name]->addChild(op);
                } else {
                    tables_operations[str_table_name] = new OrUnion();
                    tables_operations[str_table_name]->addChild(op);
                }
                toposort->addNode(table->getTableName());
                if(p == 0 || p != index->getTable()) {
                  

                    //parent = table_in;
                    //parent->addChild(op);
                    p = index->getTable();
                } else {
                    //parent->addChild(op);
                }
            }
            this->root = toposort->performTopoSort(&tables_operations);*/
        }
    }

    printf("===============\n");
    cout << typeid(*root).name() << endl;
    printf("Cost: %f, NT: %lf\n", this->root->getTotalCost(database), this->root->getNt());
    printf("===============\n");
}

vector<vector<expression_info*> > Select::getAreas(node *root) {
    vector<vector<expression_info*>> res;
    if(root == 0) return res;
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
    
    if(root == 0) return;
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
            //printf("Table: %s\n", v.table);
            Table table = *database->getTable(v.table);
            tables_set.insert(table);
        }
    }
    
}


node* Select::de_morgan(node* root) {
    
    if(root == 0) return root;
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


void Select::addSort(int numOfVariables) {
    this->pipeline = false;
    Sort* sort = new Sort();
    sort->addChild(this->root);
    this->root = sort;
}
void Select::addGroup(int numOfVariables) {
    this->pipeline = false;
    Group* group = new Group(numOfVariables);
    group->addChild(this->root);
    this->root = group;
}

void Select::addLimit(int limit) {
    this->limit = limit;
}

void Select::addHaving(int num) {
    Filter* filter = new Filter(true, num);
    filter->addChild(this->root);
    this->root = filter;
}

void Select::addSibling(Select* sibling) {
    printf("          NEW SIBLING\n");
    siblings.push_back(sibling);
}
void Select::addKid(Select* kid) {
    if(kid == 0) return;
    printf("NEW KID\n");
    kids.push_back(kid);
}

resursi Select::mergeResource(resursi A, resursi B) {
    set<string> full_scan_tables; 
    set<Index*, index_pointer_cmp> indexes;
    set<string> retr_tables;

    for(Index* index : get<1>(A)) {
        indexes.insert(index);
    }
    for(Index* index : get<1>(B)) {
        indexes.insert(index);
    }

    for(string table : get<0>(A)) {
        full_scan_tables.insert(table);
    }
    for(string table : get<0>(B)) {
        full_scan_tables.insert(table);
    }

    for(string table : get<2>(A)) {
        if(full_scan_tables.find(table) != full_scan_tables.end()) {
            continue;
        }
        retr_tables.insert(table);
    }

    return make_tuple(full_scan_tables, indexes, retr_tables);
}


int Select::sumOperations(node* root) {
    if(root == 0) return 0;
    if(root->terminal) {
        return root->e1->oper;
    }
    if(root->e1 == 0) return 0;
    return root->e1->oper + sumOperations(root->left) + sumOperations(root->right);
}

vector<Table*> Select::getTables(Database* database, map<string, Operation*> tables_operations, Operation* operation) {
    set<string>S;
    
    for(auto it = tables_operations.begin(); it != tables_operations.end(); ++it) {
        if(it->second == operation) {
            S.insert(it->first);
        } 
    }
    vector<Table*> tables;
    for(string table_name : S) {
        Table* table = database->getTable(table_name.c_str());
        tables.push_back(table);
    }
    return tables;
}

std::vector<pair<bool, int> > Select::numOfFilter(vector<Table*> tables, std::vector<expression_info*> area) {
    vector<pair<bool, int>> result_arr;
    
    vector<string> str_tables;
    for(Table* table : tables) str_tables.push_back(string(table->getTableName()));

    for(expression_info* exp : area) {
        if(exp->hasOnlyFromTables(str_tables)) {
            result_arr.push_back(make_pair(exp->equal == 1, exp->oper));
        }
    }
    return result_arr;
}

void Select::setNewOperation(Operation* old_operation , Operation* new_operation, std::map<std::string, Operation*> *operations) {
    for(auto it = operations->begin(); it != operations->end(); ++it) {
        if(old_operation == (*operations)[it->first]) {
            (*operations)[it->first] = new_operation;
        }
    }
    return;
}

void Select::setCorrelated() {
    this->correlated = true;
}

bool Select::getCorrelated() {
    return this->correlated;
}

void Select::setQuery(char* query) {
    strcpy(this->query, query);
}
pair<long, long> Select::getProcessResult() {
    return make_pair(this->timeToWait, this->timeToProcess);
}

void Select::setType(int type) {
    this->type = type;
}

int Select::getType() {
    return this->type;
}

void Select::startProcess() {
    this->startProcessing = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}
long long Select::getTimeStartProcess() {
    return this->startProcessing;
}

char* Select::getQuery() {
    return this->query;
}