#include "../db/program.h"
#include <stdexcept>
#include <iostream>
#include <libpq-fe.h>
#include <sys/time.h>
#include <tuple>
#include <string>
#include <unistd.h>
#include <chrono>
#include <pthread.h>

#include "result.h"
#include "database.h"
#include "cache.h"
#include "table.h"
#include "foreign-key.h"

using namespace web;

pthread_t Database::threads[100];

struct cmp_select_pointer { 
    bool operator()(Select* A, Select* B) 
    { 
        return A->_cost > B->_cost;
    } 
}; 

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
    //init_constants();
    //printf("%lf %lf %lf %lf %lf\n", CPU_TUPLE_COST, CPU_INDEX_TUPLE_COST, CPU_OPERATOR_COST, SEQ_PAGE_COST, RANDOM_PAGE_COST);
    //0.232150 0.749760 0.151315 1581.436827 15.676298
    
    /*CPU_TUPLE_COST = 0.0909334;
    CPU_INDEX_TUPLE_COST = 0.060808;
    CPU_OPERATOR_COST = 0.0220249;
    SEQ_PAGE_COST = 0.32956;
    RANDOM_PAGE_COST = 10.8;*/

    CPU_TUPLE_COST = 0.1025;
    CPU_INDEX_TUPLE_COST = 0.060972;
    CPU_OPERATOR_COST = 0.0364;
    SEQ_PAGE_COST = 10;
    

    
    //set one worker per query
    executeQuery("SET max_parallel_workers_per_gather = 0;");
    //BLOCK SIZE
    PGresult* result = executeQuery("show block_size;");
    BLOCK_SIZE = atoi(PQgetvalue(result, 0, 0));
    CACHE_SIZE = 0;

    result = executeQuery("show shared_buffers;");
    char* _cache = PQgetvalue(result, 0, 0);
    for(int i = 0, len = strlen(_cache); i < len - 1; ++i) {
        if(_cache[i] >= '0' && _cache[i] <= '9') {
            CACHE_SIZE = CACHE_SIZE * 10 + (_cache[i] - '0');
        } else if(_cache[i] == 'K') CACHE_SIZE *= 1024;
        else if(_cache[i] == 'M') CACHE_SIZE *= 1024 * 1024;
        else if(_cache[i] == 'G') CACHE_SIZE *= 1024 * 1024 * 1024;
    }
    
    setCacheSize(CACHE_SIZE);
    //setup cache system
    cache = new Cache(this);

}

Database::Database(web::json::value _json, int id) : Database(
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

        for(auto foreign_key_json : json["foreign_keys"].as_array()) {
            ForeignKey* key = new ForeignKey(foreign_key_json);
            foreign_keys.push_back(key);
        }
        
    } catch (const std::exception& e) {
        std::wcout << e.what() << endl;
    }

    this->id = id;
    if (pthread_mutex_init(&(this->mutex), NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return;
    } 
    printf("Mutex_init::%d\n", &mutex);
    
    //this->Q = new priority_queue<Select*, vector<Select*>, cmp_select_pointer>();
    
    pthread_create(&Database::threads[id], NULL, &(Database::threadFun), (void*)this);


}

void *Database::threadFun(void *arg) {
    Database* database = (Database*)arg;
    //auto P = database->Q2;//(priority_queue<Select*, vector<Select*>, cmp_select_pointer>*) database->Q;
    while(true) {
        usleep(10);
        pthread_mutex_lock(&(database->mutex));
        //printf("Database %d: %d\n", database->id, database->Q.size());
        if(database->Q2.size() == 0) {
            pthread_mutex_unlock(&(database->mutex));
            continue;
        }
        auto top = database->Q2.front();
        pthread_mutex_unlock(&(database->mutex));
        top->startProcess();
        PGresult *res = database->executeQuery(top->getQuery());
        database->removeQueryFromQueue();
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
    transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower);

    if(this->tables.find(table_name) != this->tables.end()) {
        printf("%s\n", t->getTableName());
        throw std::invalid_argument("Table is already existing.");
    }
    this->tables[table_name] = t;
}

bool Database::isForeignKey(const char* table1, vector<char*> columns_1, const char* table2, vector<char*> columns_2) {
    printf("isFKey %d %d %s %s\n", columns_1.size(), columns_2.size(), table1, table2);

    for(int i = 0, len = foreign_keys.size(); i < len; ++i) {
        ForeignKey* key = foreign_keys[i];
        if(strcmp(key->getTable1(), table1) == 0 
        && strcmp(key->getTable2(), table2) == 0
        && key->contains(columns_1, columns_2)) return true;
    }
    return false;
}

void Database::setCacheSize(signed int cache_size) {
    this->cache_size = cache_size;
}

void Database::addForeignKey(ForeignKey* key) {
    this->foreign_keys.push_back(key);
}

std::vector<ForeignKey*> Database::getForeignKeys() {
    return this->foreign_keys;
}

Table* Database::getTable(const char* name) {
    string table_name(name);
    transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower);

    if(this->tables.find(table_name) == this->tables.end()) {
       return 0;
       // cout << "Table: " << table_name << endl;
     //   throw std::invalid_argument("Table doesn't exsist.\n");
    }
    return this->tables[table_name];
}

char * Database::getTableNameByVar(char* variable, vector<table_name*>* tables) {
    for(table_name* _table : *tables) {
        Table* table = this->getTable(_table->real_name);
        if(table->isColumn(variable)) {
            return _table->real_name;
        }
    }
    return NULL;
}

signed int Database::getNumOfTables() {
    return this->tables.size();
}

long long Database::getCacheSize() {
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

    json["foreign_keys"] = json::value::array(foreign_keys.size());
    i = 0;
    for(ForeignKey* key : foreign_keys) {
        json["foreign_keys"][i++] = key->getJSON();
    }
    printf("foreign len %d\n", foreign_keys.size());
    return json;
}

long long Database::statusLoaded(const char* table_name) {
    Table* table = getTable(table_name);
    long long size = cache->getSize(table);
    return size;
}

long long Database::statusLoaded(Index* index) {
    return index->getSize();
}

void Database::loadInCache(Index *index) {
    cache->addNode(index);
}

void Database::loadInCache(const char* table_name, bool full) {
    Table* table = getTable(table_name);
    cache->addNode(table, full);
}

long long Database::getCurrRamLoaded(vector<Table*> full_table
                                    , vector<Index*> indexes
                                    , vector<Table*> retr_data) {
    long long ans = 0;
    printf("            RAM STATUS\n");
    for(Table* table : full_table) {
        printf("full %s %lld, %lld\n", table->getTableName(), statusLoaded(table->getTableName()), cache_size);
        ans += statusLoaded(table->getTableName());
    }
    for(Index* index : indexes) {
        printf("index %s %lld, %lld\n", index->getTable(), statusLoaded(index), cache_size);
        ans += statusLoaded(index);
    }
    for(Table* table : retr_data) {
        printf("retr %s %lld, %lld\n", table->getTableName(), statusLoaded(table->getTableName()), cache_size);
        ans += statusLoaded(table->getTableName()) / 100000;
    }

    return ans;
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
    
    
    int nt = 1500000;

    int relation_size = 213770240;
    int blocks = relation_size / BLOCK_SIZE;

    
    init_seq_page_cost(blocks);
    init_cpu_tuple_cost(nt);
    

    double sum = 0;
    for(int i = 0; i < REPEAT; ++i) sum += getTimeForQuery("SELECT * FROM orders;");
    double t1 = sum / REPEAT;
    
    t1 -= (nt * CPU_TUPLE_COST);
    
    //FINAL_TUPLE = (blocks2 * t1 - blocks * t2) / (nt * blocks2 - nt2 * blocks); 
    SEQ_PAGE_COST = (t1) / blocks;


    init_cpu_operator_cost(nt);
    init_cpu_index_tuple_cost();
    
    
    init_random_page_cost();

    printf("tuple, index, operator seq final\n");
    printf("%f %f %f %f %f %f\n", CPU_TUPLE_COST, CPU_INDEX_TUPLE_COST, CPU_OPERATOR_COST, SEQ_PAGE_COST, FINAL_TUPLE, RANDOM_PAGE_COST);
    return;
}

void Database::init_seq_page_cost(int blocks) {
    
}

void Database::init_cpu_tuple_cost(int nt) {

    executeQuery("SELECT * FROM orders;");
    double sum = 0;
    for(int i = 0; i < REPEAT; ++i) sum += getTimeForQuery("SELECT sum(o_shippriority) FROM orders;");
    double t1 = sum / REPEAT;

    sum = 0;
    for(int i = 0; i < REPEAT; ++i) sum += getTimeForQuery("SELECT sum(o_shippriority) FROM orders group by o_shippriority;");
    double t2 = sum / REPEAT;

    double a1 = nt;
    double b1 = nt;
    double a2 = nt;
    double b2 = nt + nt + nt;

    CPU_OPERATOR_COST = (a2*t1 - a1*t2) / (b1*a2 - b2*a1);
    CPU_TUPLE_COST = (t1 - a2 * CPU_OPERATOR_COST ) / a1;
}

void Database::init_cpu_operator_cost(int nt) {

}

void Database::init_cpu_index_tuple_cost() {
    
    executeQuery("set enable_seqscan = OFF;");
    PGresult *result = executeQuery("SELECT count(*) FROM orders WHERE o_custkey >= 61 and o_custkey < 45695;");
    int ni = atoi(PQgetvalue(result, 0, 0));
    double no = ni;
    double nt = no;
    
    double sum = 0;
    for(int i = 0; i < REPEAT; ++i) sum += getTimeForQuery("SELECT sum(o_custkey) FROM orders WHERE o_custkey >= 61 and o_custkey < 45695;");
    double time = sum / REPEAT;

    printf("TIME:%lf\n", time);
    CPU_INDEX_TUPLE_COST = ( time - CPU_TUPLE_COST * nt - CPU_OPERATOR_COST * no * 3) / ni;
    executeQuery("set enable_seqscan = ON;");
}



void Database::init_random_page_cost() {
    executeQuery("set enable_seqscan = OFF;");
    PGresult *result = executeQuery("SELECT count(*) FROM orders WHERE o_custkey >= 45695 and o_custkey < 105695;");
    int nt = atoi(PQgetvalue(result, 0, 0));
    int size = atoi(PQgetvalue(executeQuery("SELECT sum(pg_column_size(orders)) FROM orders WHERE o_custkey >= 45695 and o_custkey < 105695;"), 0 , 0));
    double sum = 0;
    for(int i = 0; i < REPEAT; ++i) sum += getTimeForQuery("SELECT sum(o_totalprice) FROM orders WHERE o_custkey >= 45695 and o_custkey < 105695;");
    double time = sum / REPEAT;
    int block = size / BLOCK_SIZE;
    RANDOM_PAGE_COST = (time - nt * CPU_TUPLE_COST - nt * CPU_INDEX_TUPLE_COST - 3 * nt * CPU_OPERATOR_COST) / block;
    executeQuery("set enable_seqscan = ON;");
}

double Database::getTimeForQuery(char *query) {
    auto start = std::chrono::high_resolution_clock::now();
    PGresult* res = executeQuery(query);
    free(res);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    
    return microseconds;
}

void Database::removeQueryFromQueue() {

    Program* program = Program::getInstance();
    pthread_mutex_lock(&mutex);
    //auto P = Q2;//(priority_queue<Select*, vector<Select*>, cmp_select_pointer>*) Q;
    auto top = Q2.front();
    totalCost -= (top->_cost / 1000) ;
    Q2.pop();
    pthread_mutex_unlock(&mutex);
    char buff[200];
    unsigned long long current_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
    
    long long time_wait = top->getTimeStartProcess() - top->created;
    
    long long time_process = current_time - top->getTimeStartProcess();
    sprintf(buff, "Query %d is done from replica %d - (%lld,%lld,%lf)", top->getType(), this->id, time_wait, time_process, top->_cost / 1000);
    program->log(LOG_EMERG, buff);
    
}

void Database::addRequest(Select* select) {
    Program* program = Program::getInstance();
    pthread_mutex_lock(&(this->mutex));
    unsigned long long start = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
    long long cost = select->_cost;
	//select startInLine startToProcess cost
    //auto P = (priority_queue<Select*, vector<Select*>, cmp_select_pointer>*) Q;

    Q2.push(select);   
    select->created = start;
    totalCost += cost / 1000;

    char buff[200];
    sprintf(buff, "Added query %d in list of replica %d", select->getType(), this->id);
    
    program->log(LOG_EMERG, buff);
    
    resursi reources = select->getResource();
    for(Index* in : get<1>(reources)) this->loadInCache(in);
    
    for(std::string table : get<0>(reources)) {
        this->loadInCache(table.c_str(), true);
    }

    for(std::string table : get<2>(reources)) {
        this->loadInCache(table.c_str(), false);
    }
    
    pthread_mutex_unlock(&(this->mutex));

}

long long Database::getTimeToProcess(Select *select) {
    pthread_mutex_lock(&mutex);
    //auto P = (priority_queue<Select*, vector<Select*>, cmp_select_pointer>*) Q;
    double answer = 0;
    if(Q2.size() > 0) {
        unsigned long long startedToProcess = Q2.front()->getTimeStartProcess();
        long long cost = Q2.front()->_cost;
        unsigned long long currentTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

        if(startedToProcess + cost > currentTime) {
            answer += (startedToProcess + cost) - currentTime;
        }
        for(long long cost : D) {
            answer += cost;
        }
    }
    answer += select->getFinalCost(this);

    return answer;
}

int Database::getNumOfActiveRequests() {
    //auto P = (priority_queue<Select*, vector<Select*>, cmp_select_pointer>*) Q;
    pthread_mutex_lock(&mutex);
    int size = Q2.size();
    pthread_mutex_unlock(&mutex);
    return size;
}