#include "../db/program.h"
#include <stdexcept>
#include <iostream>
#include <libpq-fe.h>
#include <sys/time.h>
#include <chrono>

#include "database.h"
#include "cache.h"
#include "table.h"
#include "foreign-key.h"

using namespace web;

Database::Database() {
}

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

    connect();
    init_constants();

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
    cout << program->data_location << endl;
    try { 
        auto json = web::json::value::parse(data);
        
        for(auto table : json["tables"].as_array()) {
            Table* t = new Table(table);
            this->tables[string(t->getTableName())] = t;
        }
    } catch (const std::exception& e) {
        std::wcout << e.what() << endl;
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

bool Database::isForeignKey(char* table1, vector<char*> columns_1,char* table2, vector<char*> columns_2) {
    ForeignKey *possible_key = new ForeignKey(table1, columns_1, table2, columns_2);
    for(ForeignKey* key : foreign_keys) {
        if(key->getHash() == possible_key->getHash() && key->equalTo(possible_key)) return true;
    }
    return false;
}

void Database::setCacheSize(signed int cache_size) {
    this->cache_size = cache_size;
}

void Database::addForeignKey(ForeignKey* key) {
    this->foreign_keys.push_back(key);
}

Table* Database::getTable(const char* name) {
    string table_name(name);
    if(this->tables.find(table_name) == this->tables.end()) {
        throw std::invalid_argument("Table doesn't exsist.\n");
    }
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
    float ratio = cache->getRatio(table);
    return table->getSize() * ratio;
}

signed int Database::statusLoaded(Index* index) {
    float ratio = cache->getRatio(index);
    return index->getSize() * ratio;
}

void Database::loadInCache(Index *index) {
    cache->addNode(index);
}

void Database::loadInCache(const char* table_name, bool full) {
    Table* table = getTable(table_name);
    cache->addNode(table, full);
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

float Database::getRatioInCache(const char* table_name) {
    Table* table = getTable(table_name);
    return cache->getRatio(table);
}

float Database::getRatioInCache(Index *index) {
    return cache->getRatio(index);
}

void Database::init_constants() {
    
    //set one worker per query
    executeQuery("SET max_parallel_workers_per_gather = 0;");
    //BLOCK SIZE
    PGresult* result = executeQuery("show block_size;");
    BLOCK_SIZE = atoi(PQgetvalue(result, 0, 0));
    //CACHE SIZE

    result = executeQuery("show shared_buffers;");
    char* cache = PQgetvalue(result, 0, 0);
    for(int i = 0, len = strlen(cache); i < len - 1; ++i) {
        if(cache[i] >= '0' && cache[i] <= '9') {
            CACHE_SIZE = CACHE_SIZE * 10 + (cache[i] - '0');
        } else if(cache[i] == 'K') CACHE_SIZE *= 1024;
        else if(cache[i] == 'M') CACHE_SIZE *= 1024 * 1024;
        else if(cache[i] == 'G') CACHE_SIZE *= 1024 * 1024 * 1024;
    }
    
    //INIT_TABLE
    
    result = executeQuery("select count(*) from R limit 3;");
    int rec_count = PQntuples(result);
    int col_count = PQnfields(result);
    
    int nt = 10000000;
    int relation_size = 362479616;
    int blocks = relation_size / BLOCK_SIZE;

    if(rec_count == 0 && col_count == 0) {
        executeQuery("DROP TABLE R;");
        executeQuery("CREATE TABLE R ( A integer, B integer);");
        executeQuery("INSERT INTO R SELECT random() * 100000, random() * 100000 FROM generate_series(1, 1000000);");
        executeQuery("CREATE INDEX r_i1 ON R (A);");
    }

    
    init_seq_page_cost(blocks);
    init_cpu_tuple_cost(nt);
    
    double temp = SEQ_PAGE_COST;
    SEQ_PAGE_COST = ( temp  - nt * CPU_TUPLE_COST - nt * FINAL_TUPLE) / blocks;

    init_cpu_operator_cost(nt);
    init_cpu_index_tuple_cost();
    
    
    init_random_page_cost();

    printf("%f %f %f %f\n", CPU_TUPLE_COST, CPU_INDEX_TUPLE_COST, CPU_OPERATOR_COST, SEQ_PAGE_COST);
    return;
}

void Database::init_seq_page_cost(int blocks) {
    SEQ_PAGE_COST = getTimeForQuery("SELECT * FROM R;");
}

void Database::init_cpu_tuple_cost(int nt) {
    executeQuery("SELECT * FROM R;");
    double time = getTimeForQuery("SELECT * FROM R;");
    printf("tuple %lf\n", time);
    CPU_TUPLE_COST = time / (double) nt;
    CPU_TUPLE_COST /= 2;
    FINAL_TUPLE = CPU_TUPLE_COST;
}

void Database::init_cpu_operator_cost(int nt) {
    double time = getTimeForQuery("SELECT COUNT(*) FROM R;");
    printf("operator %lf %lf = %lf*%lf\n", time, CPU_TUPLE_COST * nt, CPU_TUPLE_COST, nt);
    CPU_OPERATOR_COST = (time - CPU_TUPLE_COST * nt - 1 * FINAL_TUPLE) / (double) nt;
}

void Database::init_cpu_index_tuple_cost() {
    PGresult *result = executeQuery("SELECT COUNT(*) FROM R WHERE R.A < 150");
    int ni = atoi(PQgetvalue(result, 0, 0));
    int no = ni;
    int nt = no;

    double time = getTimeForQuery("SELECT * FROM R WHERE R.A < 150");
    CPU_INDEX_TUPLE_COST = ( time - CPU_TUPLE_COST * nt - CPU_OPERATOR_COST * no - FINAL_TUPLE) / (double)ni;
}



void Database::init_random_page_cost() {
    double time = getTimeForQuery("SELECT * FROM R WHERE R.B < 150;");
    PGresult *result = executeQuery("SELECT count(*) FROM R WHERE R.B < 150;");
    int nt = atoi(PQgetvalue(result, 0, 0));

}

double Database::getTimeForQuery(char *query) {
    auto start = std::chrono::high_resolution_clock::now();
    executeQuery(query);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    
    return microseconds;
}