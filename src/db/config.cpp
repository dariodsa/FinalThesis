
#ifndef CONFIG_CPP
#define CONFIG_CPP

#include "config.h"

extern "C"{
#include "../../data/y.tab.c"
#include "../../data/lex.yy.c"
}

#define ARG_NUMBERS 2
#define MAX_PORT 65535

bool connect_and_listen(char *ip, int port, std::vector<Database*> replicas) {
    Program* program = Program::getInstance();

    if(port < 0 || port > MAX_PORT) {
        program->log(LOG_WARNING, "Port number (%d) is not in valid range.\n", port);
        return false;
    }
    if(ip == NULL) {
        program->log(LOG_WARNING, "IP is not initialized.\n");
        return false;
    }

    //connect and listen to CONNECT postgres queries
    FILE *yyin = fopen("/home/dario/Documents/diplomski/FinalThesis/data/i1", "r");
    Database *database = replicas[0];
    
    printf("Call parse:\n");
    
    vector<SearchType> searchTypes;
    parse(yyin, database, &searchTypes);
    
    printf("Done parse:\n");
    
    fseek(yyin, 0, SEEK_END);
    long size = ftell(yyin);
    rewind(yyin);
    char *query = (char*)malloc(sizeof(char) * (size + 1));
    printf("Size: %d\n", size);
    for(int i = 0; i < size; ++i) {
        fread(&query[i], 1, 1, yyin);
    }
    query[size] = 0;
    //printf("%s\n", query);
    //database->executeQuery(query);
    FILE* p1 = fopen("d1", "w");
    fprintf(p1, "%s\n", database->getJSON().serialize().c_str());
    
    return true;
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

    for(auto replica : replicas_array) {
        

        printf("Adding database info: IP: %s,  port:%d\n"
                                    , (char*)replica["ipAddress"].as_string().c_str()
                                    , replica["port"].as_integer()
        );
        program->log(LOG_INFO, "Adding database info: IP: %s,  port:%d\n"
                                    , (char*)replica["ipAddress"].as_string().c_str()
                                    , replica["port"].as_integer()
        );
        replicas.push_back(new Database(
                                        replica["ipAddress"].as_string().c_str()
                                        , replica["dbName"].as_string().c_str()
                                        , replica["port"].as_integer()
                                        , replica["username"].as_string().c_str()
                                        , replica["password"].as_string().c_str()
                                        )
        );
        
    }

    closelog();
    return replicas;
 }

#endif