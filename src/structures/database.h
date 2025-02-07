#ifndef MAX_LEN
#define MAX_LEN 50
#endif

#ifndef MAX_INTEGER
#define MAX_INTEGER 0x3f3f3f3f
#endif

#ifndef DATABASE_H
#define DATABASE_H

#include <string.h>
#include <pqxx/pqxx> 
#include <stdio.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <cpprest/json.h>
#include <pthread.h>

#include "libpq-fe.h"


using namespace pqxx;
using namespace std;


class Cache;
class Select;
class Table;
class ForeignKey;
class Index;

struct variable {

    int depth;
    char name[100];
    bool t;
    char table[100];
    variable() {}
    variable(const char* _name, int _depth) {
        depth = depth;
        strcpy(name, _name);
        memset(table, 0, 100 * sizeof(char));
        t = false;
    }
    variable(const char* _name, char* _table, int _depth) {
        depth = depth;
        strcpy(name, _name);
        t = true;
        strcpy(table, _table);
    }
    void setTable(char *_table) {
        memset(table, 0, 100);
        strcpy(table, _table);
    }
};

struct table_name {

    int depth;
    char name[100];
    char real_name[100];
    table_name(const char* _name, const char* _real_name, int _depth) {
        depth = _depth;
        strcpy(name, _name);
        strcpy(real_name, _real_name);
    }
};


enum SearchType{
      SELECT_TYPE
    , INSERT_TYPE
    , CREATE_TYPE
    , ALTER_TYPE
};

struct select_state {
    std::vector<table_name*>* tables;
    std::vector<variable>* select_variable;
    bool SELECT_LIST = false;
    select_state() {
        SELECT_LIST = true;
        select_variable = new std::vector<variable>();
    }
};

struct expression_info {
    int equal = 0;
    int oper = 0;
    std::vector<variable>* variables;
    bool locked;
    expression_info() {
        variables = new std::vector<variable>();
        locked = false;
    }
    bool hasFromTables(vector<string> tables) {
        if(tables.size() == 0) return true;
        for(variable var : *variables) {
            for(string table_name : tables) {
                if(strcmp(table_name.c_str(), var.table) == 0) {
                    return true;
                }
            }
        }
        return false;
    }
    bool hasOnlyFromTables(vector<string> tables) {
        std::set<string> tab;
        for(variable var : *variables) {
            bool found = false;
            for(string table_name : tables) {
                if(strcmp(table_name.c_str(), var.table) == 0) {
                    found = true;
                    tab.insert(table_name);
                    break;
                }
            }
            if(!found) return false;
        }
        if(tab.size() == tables.size()) return true;
        return false;
    }
    bool hasFromMoreTables(vector<string> tables) {
        std::set<string> tab;
        for(variable var : *variables) {
            bool found = false;
            for(string table_name : tables) {
                if(strcmp(table_name.c_str(), var.table) == 0) {
                    found = true;
                    tab.insert(table_name);
                    break;
                }
            }
            if(!found) return false;
        }
        return true;
    }
    bool hasVariable(const char* table, const char *var) {
        for(variable _var : *variables) {
            if(strcmp(_var.name, var) == 0 && strcmp(_var.table, table) == 0) {
                return true;
            }
        }
        return false;
    }
};

struct node {
    bool terminal = false;
    char name[5];
    expression_info* e1;
    node *left = 0; 
    node *right = 0;
    node() {
        terminal = false;
        memset(name, 0, 5 * sizeof(char));
    }
};
  

class Database {
    public:
        Database();
        Database(const char *ipAddress, const char* dbName, int port, const char* username, const char* password);
        Database(web::json::value json, int id);

        bool connect();
        bool disconnect();
        void addTable(Table *t);
        PGresult* executeQuery(const char* query);

        Table* getTable(const char* table_name);
        char * getTableNameByVar(char* variable, vector<table_name*>* tables);

        signed int getNumOfTables();
        long long getCacheSize();
        void setCacheSize(signed int cache_size);

        web::json::value getJSON();

        long long statusLoaded(const char* table_name);
        long long statusLoaded(Index* index);

        /*float getRatioInCache(const char* table_name);
        float getRatioInCache(Index* index);*/

        

        void loadInCache(const char* table_name, bool full);
        void loadInCache(Index* index);

        long long getCurrRamLoaded(vector<Table*> full_table
                                  , vector<Index*> indexes
                                  , vector<Table*> retr_data);
        void addForeignKey(ForeignKey* key);

        bool isForeignKey(const char* table1, std::vector<char*> columns_table1, const char* table2, std::vector<char*> columns_table2);        

        std::vector<ForeignKey*> getForeignKeys();

        void addRequest(Select* select);
        long long getTimeToProcess(Select* select);
        int getNumOfActiveRequests();

        double SEQ_PAGE_COST;
        double RANDOM_PAGE_COST;
        double CPU_TUPLE_COST;
        double CPU_INDEX_TUPLE_COST;
        double CPU_OPERATOR_COST;

        double FINAL_TUPLE;

        int REPEAT = 5;

        int CACHE_SIZE;
        int BLOCK_SIZE;

        int id;

        double totalCost = 0;

        std::queue<Select*>Q2;
        //void* Q;
        pthread_mutex_t mutex;
        void removeQueryFromQueue();

    private:

        void init_constants();        

        void init_seq_page_cost(int blocks);
        void init_cpu_tuple_cost(int nt);
        void init_cpu_index_tuple_cost();
        void init_cpu_operator_cost(int nt);
        void init_random_page_cost();

        double getTimeForQuery(char *query);

        

        
        static pthread_t threads[100];
        static void *threadFun(void *arg);

        std::deque<long long>D;

        char dbName[MAX_LEN];
        char ipAddress[MAX_LEN];
        char username[MAX_LEN];
        char password[MAX_LEN];

        int port;

        signed int cache_size;
        Cache* cache;

        PGconn* C;

        std::map<string, Table*>tables;
        std::vector<ForeignKey*> foreign_keys;
};

#endif