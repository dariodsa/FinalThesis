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
    printf("init cost %lf\n", cost);
    for(Select* sibling : siblings) {
        if(sibling == 0 ) continue;
        if(sibling->getCorrelated()) {
             cost += sibling->getCost(database);
        } else {
            if(sibling != 0 ) cost += database->CPU_OPERATOR_COST * (log2(max(sibling->root->getNt(),this->root->getNt()))) * (this->root->getNt() + sibling->root->getNt()) + sibling->getCost(database);
        }
    }
    
    double nt = this->root->getNt();
    for(Select* kid : kids) {
        if(kid == 0) continue;
        double select_cost = kid->getCost(database);
        printf("cost in subquery %lf * %lf\n", select_cost,nt);
        printf("precost %lf nt:%lf\n", cost, nt);
        cost += (nt * select_cost);
        printf("fin cost %lf\n", cost);
    }

    printf("Res: %lf, NT: %lf\n", cost, nt);
    return cost;
}

double Select::getLoadingCost(Database* database) {
    resursi R = this->getResource();
    vector<Table*> t1;
    vector<Table*> retr_t1;
    vector<Index*> i1;
    
    long long size = 0;
    for(std::string table : get<0>(R)) {
        Table* _t = database->getTable(table.c_str());
        size += _t->getSize();
        t1.push_back(_t);
    }
    for(std::string table : get<2>(R)) {
        Table* _t = database->getTable(table.c_str());
        size += _t->getSize() / 100000;
        retr_t1.push_back(_t);
    }

    printf("GET RESOURCE %d %d %d\n", get<0>(R).size(), get<1>(R).size(), get<2>(R).size());
    set<Index*, index_pointer_cmp> s1 = std::get<1>(R);

    for(Index* _i : s1) {
        i1.push_back(_i);
    }
    
    for(Index* i : i1) {
        printf("in %s %lld\n", i->getTable(), i->getSize());
        size += i->getSize();
    }
    printf("==============  %lld %lld\n", size, database->getCurrRamLoaded(t1, i1, retr_t1));
    long long delta = database->getCurrRamLoaded(t1, i1, retr_t1);
    
    printf("          DELTA:%lld %lld %lf\n", delta, size, (double) delta /(double) size);
    return -0.5 * ( (double) delta /(double) size) + 1.0;
}

double Select::getFinalCost(Database* database) {
    double loadingCost = this->getLoadingCost(database);
    double cost = this->getCost(database);
    printf("LoadingCost: %lf\n", loadingCost);
    printf("Cost without loading: %lf\n", cost);
    double final_cost = cost * loadingCost;
    if(final_cost < 0) final_cost = 2e4;
    //printf("Final cost %f\n", final_cost);
    return final_cost;
}

resursi Select::getResource() {
    if(siblings.size() != 0 ) 
    for(Select* sibling : siblings) {
        if(sibling != 0) this->resouce = Select::mergeResource(sibling->getResource(), resouce);
    }
    for(Select* kid : kids) {
        if(kid != 0) this->resouce = Select::mergeResource(kid->getResource(), resouce);
    }
    return resouce;
}

bool Select::compare_index_pointer(pair<Index*, pair<int, int> > a, pair<Index*, pair<int, int> > b) {
    return strcmp(a.first->getTable(), b.first->getTable()) < 0;
}

Select::Select() {}

Select::Select(Database* database, node* root, vector<table_name*>* tables, vector<variable>* variables, Operation *root_like) {

    Program* program = Program::getInstance();
    this->database = database;
    //remove not nodes
    
    root = this->de_morgan(root);
    
    if(root_like != 0) {
        this->root = root_like;
        this->initResources();
        return;
    }
    
    //asign tables to the variables
    for(int i = 0, size = variables->size(); i < size; ++i) {

        variable* v = &(*variables)[i];

        if(v->table[0] == 0) {
            strcpy(v->table, database->getTableNameByVar(v->name, tables));
        } else {
            for(table_name* t : *tables) {
                if(strcmp(t->name, v->table) == 0) {
                    printf("V name %s -> %s\n", v->name, t->real_name);
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

        vector<expression_info*> area;

        auto list = getAreas(root);
        for(auto sublist : list) {
            area.insert(area.end(), sublist.begin(), sublist.end());
        }

        vector<SeqScan*> seqscans;
        for(table_name* t : *tables) {
            Table* table = database->getTable(t->real_name);
            SeqScan *_op = new SeqScan(table);
            seqscans.push_back(_op);
        }
        Operation* op =(Operation*) new NestedJoin();

        vector<Table*> tables_in_join;

        Operation* first = seqscans[0];
        vector<Table*> em;
        tables_in_join.push_back(seqscans[0]->getTable());
        
        vector<pair<bool, int> > filter_list = numOfFilter(this, tables_in_join, em, area);
        
        Operation *node = first;
        for(int i = 0, len = filter_list.size(); i < len; ++i) {
            Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
            filter->addChild(node);
            node = filter;
        }

        op->addChild(node);
        op->addChild(new Dummy());

        
        tables_in_join.push_back(seqscans[0]->getTable());
        
        

        for(int i = 1, len = seqscans.size(); i < len; ++i) {
            Operation *join = new NestedJoin();
            join->addChild(op);

            Operation* op = seqscans[i];

            vector<Table*> em;
            vector<Table*> list; list.push_back(seqscans[i]->getTable());
            
            vector<pair<bool, int> > filter_list2 = numOfFilter(this, list, em, area);
            
            Operation *_node = op;
            for(int ia = 0, lena = filter_list2.size(); ia < lena; ++ia) {
                Filter* filter = new Filter(filter_list2[ia].first, filter_list2[ia].second);
                filter->addChild(_node);
                _node = filter;
            }

            join->addChild(_node);
            
            vector<Table*> t1;
            t1.push_back(seqscans[i]->getTable());

            vector<pair<bool, int> > filter_list = numOfFilter(this, tables_in_join, t1, area);
            tables_in_join.push_back(seqscans[i]->getTable());
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
                    Network* network = new Network(this, database, table, table->getIndex(), area, &tables_operations);
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

            for(expression_info* exp : area) printf("%d %d\n",exp, exp->equal);

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
                    vector<Table*> emp;
                    vector<pair< bool, int>> filter_list = numOfFilter(this, table_list, emp, area);

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
                    //TODO
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
                        for(int k = 0; k < col1.size(); ++k) printf("%s,", col1[k]);
                        printf("\n");
                        for(int k = 0; k < col2.size(); ++k) printf("%s,", col2[k]);
                        printf("\n");
                        if(tables_operations[table2] == 0) { //desna tablica nije u join-u
                            HashJoin* hashJoin = new HashJoin(true);
                            
                            if(tables_operations[table1] == 0) {
                                Table* table = database->getTable((char*) table1.c_str());
                                Operation* op;
                                if(table->getIndexByCol(col1) == 0) op = new SeqScan(table);
                                else {
                                    Index* i1 = new Index();
                                    i1->setTable(table->getTableName());
                                    op = new IndexScan(table, i1, col1.size(), false, true);
                                }
                                vector<Table*> t1; t1.push_back(op->getTable());
                                vector<pair<bool, int>> filter_list = numOfFilter(this, Select::getTables(database, tables_operations, tables_operations[table1]), t1 ,area);
                                tables_operations[table1] = op;
                                Operation *node = op;
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
                                
                                //Index* index = table->getIndexByCol(col2);
                                Index* index = new Index();
                                index->setTable(table->getTableName());
                                
                                IndexScan* indexScan = new IndexScan(table, index, 1, 0, true);
                                vector<Table*> table_list; table_list.push_back(table);
                                vector<Table*> emp;
                                vector<pair<bool, int> > filter_list = numOfFilter(this, table_list, emp, area);
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
                        } else {

                            //desna tablica je u joinu
                            printf("DESNA TABLICA JE U JOIN\n");
                            HashJoin* join = new HashJoin(false);
                            if(tables_operations[table1] == 0) {
                                Table* table = database->getTable((char*) table1.c_str());
                                Index* index = new Index();
                                index->setTable(table1.c_str());
                                IndexScan* is = new IndexScan(table, index, 1, false, true);
                                tables_operations[table1] = is;
                            }
                            vector<Table*> tab1 = Select::getTables(database, tables_operations, tables_operations[table1]);
                            vector<Table*> tab2 = Select::getTables(database, tables_operations, tables_operations[table2]);
                            join->addChild(tables_operations[table1]);
                            join->addChild(tables_operations[table2]);
                            setNewOperation(tables_operations[table1], join, &tables_operations);
                            setNewOperation(tables_operations[table2], join, &tables_operations);

                            
                            vector<pair<bool, int> > filter_list = numOfFilter(this, tab1, tab2, area);
                            Operation *node = join;
                            for(int i = 0, len = filter_list.size(); i < len; ++i) {
                                Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                                filter->addChild(node);
                                node = filter;
                            }
                            setNewOperation(tables_operations[table1], node, &tables_operations);
                            setNewOperation(tables_operations[table2], node, &tables_operations);

                        }
                    }


                }
            }


            for(Table _table : tables_set) {
                string str_table_name = string(_table.getTableName());
                if(tables_operations[str_table_name] != 0) continue;
                
                Table* table = database->getTable(_table.getTableName());
                
                Network* network = new Network(this, database, table, table->getIndex(), area, &tables_operations);
                
                for(Table t : network->getSeqScan()) {
                    SeqScan* scan = new SeqScan(&t);
                    string table_name = string(t.getTableName());
                    vector<Table*> table_list; table_list.push_back(&t);
                    vector<Table*> emp;
                    vector<pair<bool ,int> > filter_list = numOfFilter(this, table_list, emp, area);
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
            //merge multiple join into hash join
            for(int k = 0; k < 10; ++k) {
                for(expression_info * exp : area) {
                    if(exp->equal == 1 && exp->variables->size() == 2) {
                        variable v1 = (*exp->variables)[0];
                        variable v2 = (*exp->variables)[1];
                        string t1 = string(v1.table);
                        string t2 = string(v2.table);
                        Table* tab1 = database->getTable(v1.table);
                        Table* tab2 = database->getTable(v2.table);
                        vector<char*> c1;  c1.push_back(v1.name);
                        vector<char*> c2;  c2.push_back(v2.name);
                        
                        if(tables_operations[t1] != 0 && tables_operations[t2] != 0 && tables_operations[t2] != tables_operations[t1] && tab1->getIndexByCol(c1) != 0 && tab2->getIndexByCol(c2) != 0) {
                            HashJoin* join = new HashJoin(false);
                            join->addChild(tables_operations[t1]);                             
                            join->addChild(tables_operations[t2]);
                            vector<Table*> t1_list = Select::getTables(database, tables_operations, tables_operations[t1]);
                            vector<Table*> t2_list = Select::getTables(database, tables_operations, tables_operations[t2]);
                            setNewOperation(tables_operations[t1], join, &tables_operations);
                            setNewOperation(tables_operations[t2], join, &tables_operations);

                            vector<pair<bool, int> > filter_list = numOfFilter(this, t1_list, t2_list, area);
                            Operation *node = join;
                            for(int i = 0, len = filter_list.size(); i < len; ++i) {
                                Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                                filter->addChild(node);
                                node = filter;
                            }
                            setNewOperation(join, node, &tables_operations);

                        }
                    }
                }
            }
            //Merge join search
            for(expression_info * exp : area) {
                if(exp->equal == 1 && exp->variables->size() == 2) {
                    variable v1 = (*exp->variables)[0];
                    variable v2 = (*exp->variables)[1];
                    string t1 = string(v1.table);
                    string t2 = string(v2.table);
                    Table* tab1 = database->getTable(v1.table);
                    Table* tab2 = database->getTable(v2.table);
                    if(tables_operations[t1] == 0 && tables_operations[t2] != 0) {
                        vector<char*> c1;  c1.push_back(v1.name);
                        Index* i1 = tab1->getIndexByCol(c1);

                        vector<char*> c2;  c2.push_back(v2.name);
                        Index* i2 = tab2->getIndexByCol(c2);

                        if(i1 != NULL && i2 != NULL) {
                            MergeJoin* join = new MergeJoin(false);
                            join->setSort1(true);
                            join->setSort2(true);
                            join->addChild(tables_operations[t2]);

                            IndexScan* is = new IndexScan(tab1, i1, 1, false, true);
                            join->addChild(is);
                            tables_operations[t1] = join;
                            setNewOperation(tables_operations[t2], join, &tables_operations);
                            
                            vector<Table*> v1; v1.push_back(tab1);
                            vector<pair<bool, int> > filter_list = numOfFilter(this, Select::getTables(database, tables_operations, tables_operations[t2]), v1, area);
                            Operation *node = join;
                            for(int i = 0, len = filter_list.size(); i < len; ++i) {
                                Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                                filter->addChild(node);
                                node = filter;
                            }
                            setNewOperation(join, node, &tables_operations);


                        } 
                    } else if(tables_operations[t2] == 0  && tables_operations[t1] != 0) {
                        vector<char*> c1;  c1.push_back(v1.name);
                        Index* i1 = tab1->getIndexByCol(c1);

                        vector<char*> c2;  c2.push_back(v2.name);
                        Index* i2 = tab2->getIndexByCol(c2);

                        if(i1 != NULL && i2 != NULL) {
                            MergeJoin* join = new MergeJoin(false);
                            join->setSort1(true);
                            join->setSort2(true);
                            join->addChild(tables_operations[t1]);

                            IndexScan* is = new IndexScan(tab2, i2, 1, false, true);
                            join->addChild(is);
                            tables_operations[t2] = join;
                            setNewOperation(tables_operations[t1], join, &tables_operations);
                            //setNewOperation(is,                           join, &tables_operations);

                            vector<Table*> v1; v1.push_back(tab2);
                            vector<pair<bool, int> > filter_list = numOfFilter(this, Select::getTables(database, tables_operations, tables_operations[t1]), v1, area);
                            Operation *node = join;
                            for(int i = 0, len = filter_list.size(); i < len; ++i) {
                                Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                                filter->addChild(node);
                                node = filter;
                            }
                            setNewOperation(join, node, &tables_operations);
                        }
                    }
                }
            }
            for(table_name* table_name : *tables) {
                string str_table_name = string(table_name->real_name);
                if(tables_operations[str_table_name] == 0) {
                    Table* table = database->getTable((char*) str_table_name.c_str());
                    SeqScan* seqScan = new SeqScan(table);
                    printf("Na kraju\n");
                    tables_operations[str_table_name] = seqScan;
                    vector<Table*> emp;
                    vector<pair<bool, int> > filter_list = numOfFilter(this, Select::getTables(database, tables_operations, tables_operations[str_table_name]), emp, area);
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
                cout << "pointer " << pointer << " " << first_table << endl;
                
                for(table_name* table_name : *tables) {
                    string str_table_name = string(table_name->real_name);
                    if(tables_operations[str_table_name] != pointer) {
                        NestedJoin* join = new NestedJoin();
                        join->addChild(pointer);
                        join->addChild(tables_operations[str_table_name]);
                        
                        vector<Table*> tables_in_join1 = Select::getTables(database, tables_operations, pointer);
                        vector<Table*> tables_in_join2 = Select::getTables(database, tables_operations, tables_operations[str_table_name]);

                        setNewOperation(tables_operations[str_table_name], join, &tables_operations);
                        setNewOperation(pointer,                           join, &tables_operations);
                        
                        
                        cout << "tablles in join " << str_table_name << endl;
                        vector<pair<bool, int> > filter_list = numOfFilter(this, tables_in_join1, tables_in_join2,area);
                        Operation *node = join;
                        for(int i = 0, len = filter_list.size(); i < len; ++i) {
                            Filter* filter = new Filter(filter_list[i].first, filter_list[i].second);
                            filter->addChild(node);
                            node = filter;
                        }
                        setNewOperation(tables_operations[str_table_name], node, &tables_operations);
                        setNewOperation(join,                           node, &tables_operations);
                        break;
                    }
                }
                bool done = true;
                pointer = tables_operations[first_table];
                for(table_name* table_name : *tables) {
                    if(pointer != tables_operations[string(table_name->real_name)]) done = false;
                }
                if(done) {
                    string first_table = string((*tables)[0]->real_name);
                    Operation* res_root = tables_operations[first_table]; 
                    this->root = res_root;
                    break;
                }
            }
        }
    }
    
    this->initResources();

}

void Select::initResources() {
    std::set<std::string> s1;
    std::set<Index*, index_pointer_cmp> s2;
    std::set<std::string> s3;

    if(this->root != 0) {

        queue<Operation*>Q;
        Q.push(this->root);
        int op = 0;
        while(!Q.empty()) {
            Operation* pos = Q.front();
            Q.pop();
            printf("OP: %d\n", op++);
            
            if(SeqScan* s = dynamic_cast<SeqScan*>(pos)) {
                printf("TTTTAAAABBBLLLLEEE:        ========%d  _%s_ %lld\n",s->getTable(), s->table_name, s->getTable()->getSize());
                
                s1.insert(string(s->table_name));
            } else if(IndexCon* s = dynamic_cast<IndexCon*>(pos)) {
                s2.insert(s->getIndex());
                if(s->getRetrData()) s3.insert(string(s->getIndex()->getTable()));
            } else if(IndexScan* s = dynamic_cast<IndexScan*>(pos)) {
                printf("IndexScan %d %d\n", s, s->getIndex());
                s2.insert(s->getIndex());
                if(s->getRetrData()) s3.insert(string(s->getIndex()->getTable()));
            }
        
            for(int i = 0; i < pos->children.size(); ++i) Q.push(pos->children[i]);
            
        }
        
    }
    
    std::get<0>(this->resouce) = s1;
    std::get<1>(this->resouce) = s2;
    std::get<2>(this->resouce) = s3;
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
            Table* table = database->getTable(v.table);
            if(table == 0) {}
            else tables_set.insert(*table);
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

std::vector<pair<bool, int> > Select::numOfFilter(Select* s, vector<Table*> tables1, vector<Table*> tables2, std::vector<expression_info*> area) {
    vector<pair<bool, int>> result_arr;
    
    vector<string> str_tables1;
    for(Table* table : tables1) str_tables1.push_back(string(table->getTableName()));
    vector<string> str_tables2;
    for(Table* table : tables2) str_tables2.push_back(string(table->getTableName()));
    
    for(expression_info* exp : area) {
        
        if(!s->used_exp[reinterpret_cast<intptr_t>(exp)] && exp->hasFromTables(str_tables1) && exp->hasFromTables(str_tables2)) {
            
            s->used_exp[reinterpret_cast<intptr_t>(exp)] = true;
            result_arr.push_back(make_pair(exp->equal == 1, exp->oper));
        }
    }
    return result_arr;
}

void Select::setNewOperation(Operation* old_operation , Operation* new_operation, std::map<std::string, Operation*> *operations) {
    for(auto it = operations->begin(); it != operations->end(); ++it) {
        if(old_operation == it->second) {
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