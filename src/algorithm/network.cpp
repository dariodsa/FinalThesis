#include <string.h>
#include "network.h"
#include "../structures/table.h"
#include "../structures/index.h"
#include "../structures/database.h"

#include "../structures/result.h"

#include "../structures/operations/filter.h"
#include "../structures/operations/group.h"
#include "../structures/operations/hash-join.h"
#include "../structures/operations/indexcon.h"
#include "../structures/operations/indexscan.h"
#include "../structures/operations/merge-join.h"
#include "../structures/operations/nested-join.h"
#include "../structures/operations/operation.h"
#include "../structures/operations/or-union.h"
#include "../structures/operations/seqscan.h"
#include "../structures/operations/sort.h"

using namespace std;

Network::Network(Database* database, Table* table, std::vector<Index*> indexes, vector<expression_info*> area, std::map<std::string, Operation*>* tables_operations) {
    
    for(expression_info *exp : area) {
        int cnt = 0;
        for(variable var : *exp->variables) {
            if(strcmp(var.table, table->getTableName()) == 0) ++cnt;
        }
        if(cnt > 1) {
            //SEQ SCAN na toj tablici
            Operation* scan = new SeqScan(table);
            vector<Table*> table_list; table_list.push_back(table);

            vector<int> filter_list = Select::numOfFilter(table_list, area);

            for(int i = 0, len = filter_list.size(); i < len; ++i) {
                Filter* filter = new Filter(filter_list[i]);
                filter->addChild(scan);
                scan = filter;
            }
            return;
        }
    }

    this->indexes = indexes;
    
    this->database = database;
    this->table = table;


    vector<Index*> unique_indexes;
    vector<Index*> other_indexes;

    for(Index* index : indexes) {
        if(index->getUnique()) unique_indexes.push_back(index);
        else other_indexes.push_back(index);
    }
    bool done = false;
    printf("unique %d rest %d\n", unique_indexes.size(), other_indexes.size());
    for(Index* index : unique_indexes) {
        done = useIndex(index, area, tables_operations);
        if(done) break;
    }

    if(!done)
    for(Index* index : other_indexes) {
        useIndex(index, area, tables_operations);
    }

    vector<string> t;
    t.push_back(string(table->getTableName()));

    this->retr_data = false;
    
    for(expression_info* exp : expression_infos) {
        if(exp->hasOnlyFromTables(t) && !used_expression[exp]) {
            this->retr_data = true;
        }
    }
}

bool Network::useIndex(Index* index, vector<expression_info*> expression_infos, map<string, Operation*> *tables_operations) {
    
    int cnt = 0;
    int isScan = 0;
    
    for(int col_id = 0, len = index->getColNumber(); col_id < len; ++col_id ) {
        bool found = false;
        
        for(expression_info* exp_info : expression_infos) {
            
            bool not_selected_tables = true;
            for(variable var : *(exp_info->variables)) {
                string t1 = string(var.table);
                
                if((*tables_operations)[t1] != 0) {
                    not_selected_tables = false;
                    break;
                }
            }
            if(!not_selected_tables) continue;
            
            if(exp_info->hasVariable(table->getTableName(), index->getColName(col_id))) {
            
                used_expression[exp_info] = true;
                for(variable var : *exp_info->variables) {
                    if(strcmp(var.table, table->getTableName()) != 0) {
                        seq_scan.insert(*database->getTable(var.table));
                    }
                }
                found = true;
                
                if(exp_info->equal != 1) {
                    cnt = col_id + 1;
                    isScan = 1;
                    auto P = make_pair(index, make_pair(isScan, cnt));
                    used_indexes.push_back(P);
                    return false;

                }
            }
        }
        if(!found) {
            if(col_id == 0) return false;
            cnt = col_id;
            isScan = 0;
            
            auto P = make_pair(index, make_pair(isScan, cnt));
            used_indexes.push_back(P);
            return false;
        }
    }
    
    cnt = index->getColNumber();
    isScan = 0;
    
    auto P = make_pair(index, make_pair(isScan, cnt));
    used_indexes.push_back(P);

    if(index->getUnique()) return true;
    return false;
}

bool Network::getRetrData() {
    return this->retr_data;
}

set<Table> Network::getSeqScan() {
    return this->seq_scan;
}
vector<pair<Index*, pair<int, int> >> Network::getUsedIndexes() {
    return this->used_indexes;
}