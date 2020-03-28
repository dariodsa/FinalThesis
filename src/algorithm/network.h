#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <set>
#include <utility>
#include <string>

class Index;
class Table;
class Database;

struct variable;
struct expression_info;
struct node;
struct select_state;
struct table_name;

class Network {
    public:
        Network(Database* database, Table* table, std::vector<Index*> indexes, std::vector<expression_info*> expression_infos, std::vector<std::string> indexed_tables);

        std::set<Table> getSeqScan();
        std::vector<std::pair<Index*, std::pair<int, int> > > getUsedIndexes();
        bool getRetrData();
    private:
        std::vector<std::pair<Index*, std::pair<int, int> > > used_indexes;
        std::set<Table> seq_scan;

        std::vector<Index*> indexes;
        std::vector<std::string> indexed_tables;
        std::vector<expression_info*> expression_infos;
        
        Table* table;
        Database* database;

        bool retr_data;

        void useIndex(Index* index, std::vector<expression_info*> expression_infos);

};

#endif