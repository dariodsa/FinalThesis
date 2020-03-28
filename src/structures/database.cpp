#include "../db/program.h"
#include <stdexcept>
#include <iostream>
#include <libpq-fe.h>

#include "database.h"
#include "cache.h"
#include "table.h"

using namespace web;

Database::Database() {}

Database::Database(const char *ipAddress, const char* dbName, int port, const char* username, const char* password) {
    
    setCacheSize(Program::DEFAULT_CACHE_SIZE);
    
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

    //setup cache system
    cache = new Cache(this);

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

PGresult* Database::executeQuery(const char* query) {

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

void Database::setCacheSize(signed int cache_size) {
    this->cache_size = cache_size;
}

Table* Database::getTable(const char* name) {
    string table_name(name);
    return this->tables[table_name];
}

char * Database::getTableNameByVar(char* variable, vector<table_name*>* tables) {
    for(table_name* _table : *tables) {
        Table* table = this->getTable(_table->real_name);
        printf("%s in %s\n", variable, _table->real_name);
        if(table->isColumn(variable)) {
            return _table->real_name;
        }
    }
    return NULL;
}

signed int Database::getNumOfTables() {
    return this->tables.size();
}

signed int Database::getCacheSize() {
    return this->cache_size;
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

signed int Database::statusLoaded(const char* table_name) {
    Table* table = getTable(table_name);
    return cache->getLoadedStatus(table);
}

signed int Database::statusLoaded(Index* index) {
    return cache->getLoadedStatus(index);
}

void Database::loadInCache(Index *index) {

}

void Database::loadInCache(const char* table_name) {

}

signed int Database::getCurrRamLoaded(vector<Table*> full_table
                                    , vector<Index*> indexes
                                    , vector<Table*> retr_data) {
    signed int ans = 0;
    for(Table* table : full_table) {
        ans += statusLoaded(table->getTableName());
    }
    for(Index* index : indexes) {
        ans += statusLoaded(index);
    }
    for(Table* table : retr_data) {
        ans += statusLoaded(table->getTableName());
    }

    return ans;
}