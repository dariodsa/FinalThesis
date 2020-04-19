#ifndef RESULT_H
#define RESULT_H 

#include <string.h>
#include <vector>
#include <set>
#include <tuple>
#include "index.h"

class Database;

class Operation;
class Column;

extern char AND_STR[4];
extern char NOT_STR[4];
extern char OR_STR[4];

struct index_pointer_cmp {
    bool operator()(const Index* A, const Index* B) const {
        return A->getHash() < B->getHash();
    }
};

typedef std::tuple<std::set<std::string>, std::set<Index*, index_pointer_cmp>, std::set<std::string>> resursi;


class Result{
    public:
        Result();
        Result(Table* table, std::vector<Column*>* columns);
        void setParent(Result* result);
        void setLock();
        void setEqualType(int equalType);
        void print();
        bool hasColumn(char* col_name);

        Table* getTable();
        vector<Column*> getColumns();
        Result* getParent();
        int getEqualType();

    private:
        Table *table;
        Result* parent;
        int equaltype = 0;
        bool locked;
        std::vector<Column*> columns;

};

class Select{
    public: 
        Select();
        Select(Database* database, node* root, std::vector<table_name*>* tables, std::vector<variable>* variables);

        double getFinalCost(Database *database);
        
        void setQuery(char* query);
        void setType(int type);
        int getType();
        void startProcess();
        long long getTimeStartProcess();
        std::pair<long, long> getProcessResult();
        char *getQuery();

        static bool compare_index_pointer(pair<Index*, pair<int, int> > a, pair<Index*, pair<int, int> > b);
        static resursi mergeResource(resursi A, resursi B);

        void addSort(int numOfVariables);
        void addGroup(int numOfVariables);
        void addHaving(int num);
        void addLimit(int limit);

        void setCorrelated();
        bool getCorrelated();

        void addSibling(Select* sibling);
        void addKid(Select* kid);

        resursi getResource();

        static std::vector<pair<bool, int> > numOfFilter(std::vector<Table*> tables, std::vector<expression_info*> area);
        static void setNewOperation(Operation* old_operation , Operation* new_operation, std::map<std::string, Operation*> *operations);
        static std::vector<Table*> getTables(Database* database, std::map<std::string, Operation*> tables_operations, Operation* operation);
    private:
        void dfs(node *root);
        vector<vector<expression_info*> > getAreas(node *root);
        double getCost(Database *database);
        double getLoadingCost(Database* database);

        int sumOperations(node *root);

        node* de_morgan(node* root);

        char query[2000];
        long long timeToWait;
        long long timeToProcess;
        int type;
        long long startProcessing;

        resursi resouce;
        Database* database;

        Operation* root;

        std::set<Table> tables_set;
        int or_node;
        int table_count;

        bool correlated = false;
        bool pipeline = true;
        int limit = -1;

        std::vector<Select*> siblings;
        std::vector<Select*> kids;
};



#endif