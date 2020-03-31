
#ifndef CONFIG_CPP
#define CONFIG_CPP

#include "config.h"
#include <sys/time.h>

extern "C"{
#include "../../data/y.tab.c"
#include "../../data/lex.yy.c"
}

#define ARG_NUMBERS 2
#define MAX_PORT 65535

float timedifference_msec(struct timeval t0, struct timeval t1) {
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

std::pair<float, float> connect_and_listen(char *ip, int port, std::vector<Database*>* replicas, const char* query) {
    
    Program* program = Program::getInstance();

    //connect and listen to CONNECT postgres queries
    Database *database = (*replicas)[0];

    printf("Call parse:\n");

    vector<SearchType> searchTypes;


    /*FILE* p1 = fopen("d1", "w");
    fprintf(p1, "%s\n", database->getJSON().serialize().c_str());*/

    /*FILE *f = fopen("sql/init.sql", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = (char *)malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;*/


    Select* result = parse(query, database, &searchTypes);
    cout << query << endl;
    float cost = result->getFinalCost(database);

    printf("Done parsing.\n");

    struct timeval t0;
    struct timeval t1;
    float elapsed;

    gettimeofday(&t0, 0);


    PGresult *res = database->executeQuery(query);
    
    gettimeofday(&t1, 0);

    elapsed = timedifference_msec(t0, t1);

    printf("Code executed in %f milliseconds.\n", elapsed);
    

    int rec_count = PQntuples(res);
    int col_count = PQnfields(res);
    printf("We have %d rows.\n", rec_count);
    for(int row = 0; row < rec_count; ++row) {
        for(int col = 0; col < col_count; ++col) {
            printf("%s ", PQgetvalue(res, row, col));
        }
        printf("\n");
    }
    return make_pair(cost, elapsed);
    /*fseek(yyin, 0, SEEK_END);
    long size = ftell(yyin);
    rewind(yyin);
    char *query = (char*)malloc(sizeof(char) * (size + 1));
    printf("Size: %d\n", size);
    for(int i = 0; i < size; ++i) {
        fread(&query[i], 1, 1, yyin);
    }
    query[size] = 0;
    //printf("%s\n", query);struct timeval t1;
    float elapsed;

    gettimeofday(&t0, 0);


    PGresult *res = database->executeQuery(p);
    int rec_count = PQntuples(res);
    int col_count = PQnfields(res);
    printf("We have %d rows.\n", rec_count);
    for(int row = 0; row < rec_count; ++row) {
        for(
    //database->executeQuerystruct timeval t1;
    float elapsed;

    gettimeofday(&t0, 0);


    PGresult *res = database->executeQuery(p);
    int rec_count = PQntuples(res);
    int col_count = PQnfields(res);
    printf("We have %d rows.\n", rec_count);
    for(int row = 0; row < rec_count; ++row) {
        for(query);
    FILE* p1 = fopen("d1", "struct timeval t1;
    float elapsed;

    gettimeofday(&t0, 0);


    PGresult *res = database->executeQuery(p);
    int rec_count = PQntuples(res);
    int col_count = PQnfields(res);
    printf("We have %d rows.\n", rec_count);
    for(int row = 0; row < rec_count; ++row) {
        for(");
    fprintf(p1, "%s\n", datastruct timeval t1;
    float elapsed;

    gettimeofday(&t0, 0);


    PGresult *res = database->executeQuery(p);
    int rec_count = PQntuples(res);
    int col_count = PQnfields(res);
    printf("We have %d rows.\n", rec_count);
    for(int row = 0; row < rec_count; ++row) {
        for(ase->getJSON().serialize().c_str());
    */
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