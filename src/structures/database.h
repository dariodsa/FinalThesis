#include <string.h>
#include <pqxx/pqxx> 
#include <stdio.h>
#include <map>
#include <string>
#include "table.h"
#include "libpq-fe.h"

using namespace pqxx;

#ifndef DATABASE_H
#define DATABASE_H

#define MAX_LEN 30

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
        bool executeQuery(char* query, SearchType type);

        Table* getTable(char* table_name);
        size_t getNumOfTables();

        web::json::value getJSON();

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