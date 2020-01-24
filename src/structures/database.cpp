#include "database.h"
#include <stdexcept>
#include <libpq-fe.h>

Database::Database(char *ipAddress, int port) {
    if(ipAddress == NULL) {
        throw std::invalid_argument("Ip address is pointing to zero.\n");
    }
    if(strlen(ipAddress) > MAX_LEN) {
        throw std::invalid_argument("ipAddress is too long. It should be less than \n");
    }
    this->port = port;
    strcpy(this->ipAddress, ipAddress);
}

bool Database::connect() {
    return true;
}