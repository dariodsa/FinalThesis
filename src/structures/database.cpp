#include "database.h"
#include "../db/program.h"
#include <stdexcept>
#include <libpq-fe.h>


Database::Database(char *ipAddress, char* dbName, int port, char* username, char* password) {
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
        char conn_str[150];
        
        sprintf(conn_str, "dbname = %s user = %s password = %s \
            hostaddr = %s port = %d", this->dbName, this->username, this->password, this->ipAddress, this->port);
        /*this->C = new connection(conn_str);
        if (C->is_open()) {
            return true;
        } else {   
            return false;
        }*/
    } catch (const std::exception &e) {
        program->log(LOG_WARNING, "Exception message: %s", e.what());
        return false;
    }
    return true;
}

bool Database::disconnect() {
    Program* program = Program::getInstance();
    try {
        //C->disconnect();
    } catch(const std::exception &e) {
        program->log(LOG_WARNING, "Exception message: %s", e.what());
        return false;
    }

    return true;
}