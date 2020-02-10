#include <string.h>
#include <pqxx/pqxx> 
#include <stdio.h>

using namespace pqxx;

#ifndef DATABASE_H
#define DATABASE_H

#define MAX_LEN 30

class Database {
    public:
        Database(char *ipAddress, char* dbName, int port, char* username, char* password);
        bool connect();
        bool disconnect();
    private:
    char dbName[MAX_LEN];
    char ipAddress[MAX_LEN];
    char username[MAX_LEN];
    char password[MAX_LEN];

    int port;

    pqxx::connection* C;
};

#endif