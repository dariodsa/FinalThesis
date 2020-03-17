
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
    printf("Num of tables %d\n", database->getNumOfTables());
    vector<SearchType> searchTypes;
    
    const char *p = "SELECT gi.graphid,i.hostid FROM graphs_items gi,items i WHERE fun(gi.graphid)=823 AND gi.itemid=i.itemid;";
    parse(p, database, &searchTypes);
    
    printf("Done parse:\n");
    
    
    PGresult *res = database->executeQuery(p);
    int rec_count = PQntuples(res);
    int col_count = PQnfields(res);
    printf("We have %d rows.\n", rec_count);
    for(int row = 0; row < rec_count; ++row) {
        for(int col = 0; col < col_count; ++col) {
            printf("%s ", PQgetvalue(res, row, col));
        }
        printf("\n");
    }

    /*fseek(yyin, 0, SEEK_END);
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
    */
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
    program->data_location = json["data"].as_string().c_str();

    for(auto replica : replicas_array) {
        
        const char* ipAddress = replica["ipAddress"].as_string().c_str();
        int port = replica["port"].as_integer();
        
        program->log(LOG_INFO, "Adding database info: IP: %s, port: %d\n"
                                    , ipAddress
                                    , port
                                    
        );
        replicas.push_back(new Database(replica));
        
    }

    closelog();
    return replicas;
 }

#endif