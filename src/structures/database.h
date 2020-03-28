#ifndef MAX_LEN
#define MAX_LEN 50
#endif

#ifndef DATABASE_H
#define DATABASE_H

#include <string.h>
#include <pqxx/pqxx> 
#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "libpq-fe.h"
#include "table.h"

using namespace pqxx;
using namespace std;

class Table;
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
    std::vector<variable>* variables;
    bool locked;
    expression_info() {
        variables = new std::vector<variable>();
        locked = false;
    }
    bool hasFromTables(vector<string> tables) {
        for(variable var : *variables) {
            for(string table_name : tables) {
                if(strcmp(table_name.c_str(), var.table) == 0) {
                    return true;
                }
            }
        }
        return false;
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


enum SearchType{
      SELECT_TYPE
    , INSERT_TYPE
    , CREATE_TYPE
    , ALTER_TYPE
};



class Database {
    public:
        Database();
        Database(const char *ipAddress, const char* dbName, int port, const char* username, const char* password);
        Database(web::json::value json);

        bool connect();
        bool disconnect();
        void addTable(Table *t);
        PGresult* executeQuery(const char* query);

        Table* getTable(const char* table_name);
        char * getTableNameByVar(char* variable, vector<table_name*>* tables);

        size_t getNumOfTables();

        web::json::value getJSON();

        bool isTableLoaded(const char* table_name);
        bool isIndexLoaded(Index* index);

        void loadTable(const char* table_name);
        void loadIndex(Index* index);

    private:
        char dbName[MAX_LEN];
        char ipAddress[MAX_LEN];
        char username[MAX_LEN];
        char password[MAX_LEN];

        int port;

        PGconn* C;

        std::map<string, Table*>tables;
};

#endif