#include "database.h"
#include "../db/program.h"
#include <stdexcept>
#include <iostream>
#include <libpq-fe.h>

Database::Database() {}

Database::Database(char *ipAddress, char* dbName, int port, char* username, char* password) {
    if(ipAddress == NULL) {
        throw std::invalid_argument("Ip address is pointing to zero.\n");
    }
    if(strlen(ipAddress) > MAX_LEN) {
        throw std::invalid_argument("ipAddress is too long. It should be less than \n");
    }

    this->port = port;
    strcpy(this->ipAddress, ipAddress);
    strcpy(this->password, password);
    strcpy(this->username, username);
    strcpy(this->dbName, dbName);
}

bool Database::connect() {
    Program* program = Program::getInstance();
    try {
        char conn_str[150];
        
        sprintf(conn_str, "dbname = %s user = %s password = %s \
            hostaddr = %s port = %d", this->dbName, this->username, this->password, this->ipAddress, this->port);
        printf("%s\n", conn_str);
        this->C = new connection(conn_str);
        if (C && C->is_open()) {
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

bool Database::executeQuery(char* query) {
    Program* program = Program::getInstance();
    try {
        pqxx::work W(*C);
        W.exec( query );
        W.commit();
    } catch(const std::exception &e) {
        program->log(LOG_WARNING, "Exception message: %s", e.what());
        return false;
    }
    return true;
}

void Database::addTable(Table *t) {
    string table_name(t->getTableName());
    
    if(this->tables.find(table_name) != this->tables.end()) {
        printf("%s\n", t->getTableName());
        throw std::invalid_argument("Table is already existing.");
    }
    this->tables[table_name] = t;
}
Table* Database::getTable(char* name) {
    string table_name(name);
    return this->tables[table_name];
}