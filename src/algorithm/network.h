#ifndef NETWORK_CLASS
#define NETWORK_CLASS 1

#include <vector>
#include <set>
#include "../structures/index.h"
#include "../structures/database.h"

class Network {
    public:
        Network(Database* database, Table table, std::vector<Index*> indexes, std::vector<expression_info*> expression_infos, std::vector<string> indexed_tables);

        set<Table> getSeqScan();
        vector<pair<Index*, pair<int, int> > > getUsedIndexes();
        bool getRetrData();
    private:
        std::vector<std::pair<Index*, std::pair<int, int> > > used_indexes;
        std::set<Table> seq_scan;

        std::vector<Index*> indexes;
        std::vector<string> indexed_tables;
        std::vector<expression_info*> expression_infos;
        
        Table table;
        Database* database;

        bool retr_data;

        void useIndex(Index* index, vector<expression_info*> expression_infos);

};

#endif