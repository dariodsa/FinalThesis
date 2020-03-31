#include <string.h>
#include "network.h"
#include "../structures/table.h"
#include "../structures/index.h"
#include "../structures/database.h"
using namespace std;

Network::Network(Database* database, Table* table, std::vector<Index*> indexes, vector<expression_info*> expression_infos, vector<string> indexed_tables) {
    for(expression_info *exp : expression_infos) {
        int cnt = 0;
        for(variable var : *exp->variables) {
            if(strcmp(var.table, table->getTableName()) == 0) ++cnt;
        }
        if(cnt > 1) {
            this->seq_scan.insert(*table);
            return;
        }
    }

    this->indexes = indexes;
    this->indexed_tables = indexed_tables;
    this->database = database;
    this->table = table;


    vector<Index*> unique_indexes;
    vector<Index*> other_indexes;
    for(Index* index : indexes) {
        if(index->getUnique()) unique_indexes.push_back(index);
        else other_indexes.push_back(index);
    }
    bool done = false;
    
    for(Index* index : unique_indexes) {
        done = useIndex(index, expression_infos);
        if(done) break;
    }

    if(!done)
    for(Index* index : other_indexes) {
        done = useIndex(index, expression_infos);
        if(done) break;
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

bool Network::useIndex(Index* index, vector<expression_info*> expression_infos) {
    
    int cnt = 0;
    int isScan = 0;

    for(int col_id = 0, len = index->getColNumber(); col_id < len; ++col_id ) {
        bool found = false;
        
        for(expression_info* exp_info : expression_infos) {
            
            if(exp_info->hasFromTables(indexed_tables)) continue;
            
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