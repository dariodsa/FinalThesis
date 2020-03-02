#include "database.h"
#include "../db/program.h"
#include <stdexcept>
#include <iostream>
#include <libpq-fe.h>

using namespace web;

Database::Database() {}

Database::Database(const char *ipAddress, const char* dbName, int port, const char* username, const char* password) {
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

Database::Database(web::json::value _json) : Database(
                                            _json["ipAddress"].as_string().c_str()
                                          , _json["dbName"].as_string().c_str()
                                          , _json["port"].as_integer()
                                          , _json["username"].as_string().c_str()
                                          , _json["password"].as_string().c_str()) {
    
    Program* program = Program::getInstance();
    std::ifstream data;
    data.open(program->data_location);
    
    auto json = web::json::value::parse(data);
    
    for(auto table : json["tables"].as_array()) {
        Table* t = new Table(table);
        this->tables[string(t->getTableName())] = t;
    }
}

bool Database::connect() {
    Program* program = Program::getInstance();
    try {
        char conn_str[150];
        
        sprintf(conn_str, "dbname = %s user = %s password = %s \
            hostaddr = %s port = %d", this->dbName, this->username, this->password, this->ipAddress, this->port);
        printf("%s\n", conn_str);
        this->C = PQconnectdb(conn_str);
        if (PQstatus(this->C) == CONNECTION_BAD)   {
            return false;
        } else {   
            return true;
        }
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
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

PGresult* Database::executeQuery(char* query) {

    Program* program = Program::getInstance();
    PGresult *res;
    
    try {
        
        res = PQexec(this->C, query);
        int rec_count = PQntuples(res);
        int col_count = PQnfields(res);
        
    } catch(const std::exception &e) {
        printf("%s\n", e.what());
        program->log(LOG_WARNING, "Exception message: %s", e.what());
        return NULL;
    }
    return res;
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

size_t Database::getNumOfTables() {
    return this->tables.size();
}


web::json::value Database::getJSON() {
    json::value json;
    json["dbName"] = json::value::string(string(dbName));
    json["ipAddress"] = json::value::string(string(ipAddress));
    json["username"] = json::value::string(string(username));
    json["password"] = json::value::string(string(password));

    json["port"] = json::value::number(port);

    json["tables"] = json::value::array(tables.size());
    map<string, Table*>::iterator it;
    int i = 0;
    for(it = tables.begin(); it != tables.end(); ++it, ++i) {
        json["tables"][i] = it->second->getJSON();
    }

    return json;
}