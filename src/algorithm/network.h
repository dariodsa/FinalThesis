#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <set>
#include <map>
#include <utility>
#include <string>

class Index;
class Table;
class Database;
class TopoSort;
class Operation;


struct variable;
struct expression_info;
struct node;
struct select_state;
struct table_name;

class Network {
    public:
        Network(Database* database, Table* table, std::vector<Index*> indexes, std::vector<expression_info*> expression_infos, std::map<std::string, Operation*>* tables_operations);

        std::set<Table> getSeqScan();
        std::vector<std::pair<Index*, std::pair<int, int> > > getUsedIndexes();
        bool getRetrData();
    private:
        std::vector<std::pair<Index*, std::pair<int, int> > > used_indexes;
        std::set<Table> seq_scan;

        std::vector<Index*> indexes;
        
        std::vector<expression_info*> expression_infos;

        std::map<expression_info*, bool> used_expression;

        Table* table;
        Database* database;
        
        bool retr_data;

        bool useIndex(Index* index, std::vector<expression_info*> expression_infos, std::map<std::string, Operation*> *tables_operations);

};

#endif