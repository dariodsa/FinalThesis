#include <string.h>
#include <vector>
#include "database.h"
#include "index.h"

#ifndef RESULT_H
#define RESULT_H 

extern char AND_STR[4];
extern char NOT_STR[4];
extern char OR_STR[4];

enum SEARCH_TYPE{
    INDEX_SCAN,
    INDEX_CONST,
    SEQ_SCAN
};

class Sorting{
    public:
        Sorting(int num_rows, bool disc);
    private:
        int num_rows;
        bool disc;
};

class Result{
    public:
        Result();
        Result(Table* table, std::vector<Column*>* columns);
        void removeAndFlood();
        void setLock();
        void print();
        void addElement(Result *result);
        bool hasColumn(char* col_name);

        Table getTable();
        vector<Column*> getColumns();
        vector<Result*> getResults();

    private:
        
        Table *table;
        
        SEARCH_TYPE type;
        
        bool locked;
        bool and_flood;

        std::vector<Column*> columns;
        std::vector<Result*> results;
        
};

class Select{
    public: 
        Select(Database *database, std::vector<table_name*>* tables, std::vector<variable> *variables);
        Select(Database* database, node* root, std::vector<table_name*>* tables, std::vector<variable>* variables);
    private:
        vector<Result*> dfs(node *node);

        vector<Result*> processAnd(vector<Result*> inputs);
        vector<Result*> lookForIndex(Index* index, vector<Result*> inputs);

        node* de_morgan(node* root);
        Database* database;
};



#endif