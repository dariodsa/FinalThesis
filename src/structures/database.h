#include <string.h>
#include <pqxx/pqxx> 
#include <stdio.h>

using namespace pqxx;

#define MAX_LEN 30

class Database {
    public:
        Database(char *ipAddress, int port, char* username, char* password);
        bool connect();
        bool disconnect();
    private:
    char ipAddress[MAX_LEN];
    char username[MAX_LEN];
    char password[MAX_LEN];

    int port;

    pqxx::connection* C;
};