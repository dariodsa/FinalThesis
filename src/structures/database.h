#include <string.h>
#include <pqxx/pqxx> 
#include <stdio.h>
#include <map>
#include <string>
#include "table.h"

using namespace pqxx;

#ifndef DATABASE_H
#define DATABASE_H

#define MAX_LEN 30

class Database {
    public:
        Database();
        Database(char *ipAddress, char* dbName, int port, char* username, char* password);
        bool connect();
        bool disconnect();
        void addTable(Table *t);
        bool executeQuery(char* query);
        Table* getTable(char* table_name);

        web::json::value getJSON();

    private:
    char dbName[MAX_LEN];
    char ipAddress[MAX_LEN];
    char username[MAX_LEN];
    char password[MAX_LEN];

    int port;

    pqxx::connection* C;

    std::map<string, Table*>tables;
};

enum SearchType{
      SELECT_TYPE
    , INSERT_TYPE
    , CREATE_TYPE
    , ALTER_TYPE
};

#endif