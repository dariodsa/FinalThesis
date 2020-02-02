#include "database.h"
#include "program.h"
#include <stdexcept>
#include <libpq-fe.h>


Database::Database(char *ipAddress, int port, char* username, char* password) {
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
    Program* program = Program::getInstance();
    try {
        this->C = new connection("dbname = testdb user = postgres password = cohondob \
            hostaddr = 127.0.0.1 port = 5432");
        if (C->is_open()) {
            return true;
        } else {   
            return false;
        }
    } catch (const std::exception &e) {
        program->log(LOG_WARNING, "Exception message: %s", e.what());
        return false;
    }
    return true;
}

bool Database::disconnect() {
    Program* program = Program::getInstance();
    try {
        C->disconnect();
    } catch(const std::exception &e) {
        program->log(LOG_WARNING, "Exception message: %s", e.what());
        return false;
    }

    return true;
}