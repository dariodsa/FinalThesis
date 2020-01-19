#include <string.h>

#define MAX_LEN 30

class Database {
    public:
        Database(char *ipAddress, int port);
        bool connect();
    private:
    char ipAddress[MAX_LEN];
    int port;
};