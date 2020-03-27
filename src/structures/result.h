#include <string.h>
#include <vector>
#include <set>

#include "database.h"
#include "index.h"

#include "operations/operation.h"

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
        Select(Database *database, std::vector<table_name*>* tables, std::vector<variable> *variables);
        Select(Database* database, node* root, std::vector<table_name*>* tables, std::vector<variable>* variables);
    
        float getCost();
        static bool compare_index_pointer(pair<Index*, pair<int, int> > a, pair<Index*, pair<int, int> > b);

    private:
        void dfs(node *root);
        vector<vector<expression_info*> > getAreas(node *root);
        
        node* de_morgan(node* root);
        Database* database;

        std::set<Table> tables_set;
        int or_node;
        int table_count;

        Operation *operation = new Operation(0);
};



#endif