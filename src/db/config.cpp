
#ifndef CONFIG_CPP
#define CONFIG_CPP

#include "config.h"
#include <chrono>

extern "C"{
#include "../../data/y.tab.c"
#include "../../data/lex.yy.c"
}

#define ARG_NUMBERS 2
#define MAX_PORT 65535

float timedifference_msec(struct timeval t0, struct timeval t1) {
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

Select* process_query(Database* replica, const char* query) {
    
    Program* program = Program::getInstance();

    //connect and listen to CONNECT postgres queries
    Database *database = replica;

    vector<SearchType> searchTypes;

    Select* result = parse(query, database, &searchTypes);
    
    result->setQuery((char*)query);

    return result;
}

bool connect_to_replicas(std::vector<Database*> replicas) {
    Program *program = Program::getInstance();
    int cnt = 0;
    for(Database* replica : replicas) {
        if( replica->connect() ) {
           ++cnt;
        }
    }
    return cnt > 0;
}

std::vector<Database*> setup_db_replicas_pool(web::json::value json) {

    Program *program = Program::getInstance();

    std::vector<Database*> replicas;
    
    auto replicas_array = json["replicas"].as_array();
    program->data_location = json["data"].as_string().c_str();

    int id = 0;
    for(auto replica : replicas_array) {
        
        const char* ipAddress = replica["ipAddress"].as_string().c_str();
        int port = replica["port"].as_integer();
        
        program->log(LOG_INFO, "Adding database info: IP: %s, port: %d\n"
                                    , ipAddress
                                    , port
                                    
        );
        replicas.push_back(new Database(replica, id));
        id++;
        
    }

    closelog();
    return replicas;
 }

#endif