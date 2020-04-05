#ifndef RESULT_H
#define RESULT_H 

#include <string.h>
#include <vector>
#include <set>
#include <tuple>

class Database;
class Index;
class Operation;

extern char AND_STR[4];
extern char NOT_STR[4];
extern char OR_STR[4];

struct index_pointer_cmp{
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

        float getFinalCost(Database *database);
        
        static bool compare_index_pointer(pair<Index*, pair<int, int> > a, pair<Index*, pair<int, int> > b);
        static resursi mergeResource(resursi A, resursi B);

        void addSort();
        void addGroup();
        void addLimit(int limit);

        void addSibling(Select* sibling);
        void addKid(Select* kid);

    private:
        void dfs(node *root);
        vector<vector<expression_info*> > getAreas(node *root);
        float getCost(Database *database);
        float getLoadingCost(Database* database);

        int sumOperations(node *root);

        node* de_morgan(node* root);
        
        resursi getResource();

        resursi resouce;
        Database* database;

        Operation* root;

        std::set<Table> tables_set;
        int or_node;
        int table_count;

        bool pipeline = true;
        int limit = -1;

        std::vector<Select*> siblings;
        std::vector<Select*> kids;
};



#endif