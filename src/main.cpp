#include <stdio.h>

#include "db/config.h"

#include <libpq-fe.h>
#include <vector>

#include "db/proxy.h"

using namespace std;
// list of replicas involved
vector<Database*> replicas;


pthread_mutex_t replicas_lock;

/*
 * thread which is responsible for contacting replicas 
 * and for reporting about their life
*/
pthread_t keep_alive_thread;


int main(int argc, char* argv[]) {
    
    //
    
    
    Program* program = Program::getInstance();
    
    program->setUp(argc, argv);

    char *CONFIG_FILE_PATH = program->getConfigFilePath();

    std::ifstream t;
    t.open(CONFIG_FILE_PATH);
    
    auto json = web::json::value::parse(t);

    replicas = setup_db_replicas_pool(json);
    
    if(replicas.size() == 0) {
        program->log(LOG_WARNING, "Number of databases are zero.\n");
        exit(0);
    } 
    
    if( !connect_to_replicas(replicas) ) {
        program->log(LOG_WARNING, "None of the replicas are alive.\n");
        return 0;
    }
    
    //Proxy* p = new Proxy(55432, replicas[0]);

    vector<const char*> queries;
    queries.push_back("SELECT * FROM sbtest1, sbtest2 limit 5;");
    queries.push_back("SELECT * from sbtest1 where k = 2;");
    queries.push_back("SELECT * from sbtest1 where k < 2;");
    queries.push_back("SELECT * from sbtest1 where k = 2 and id = 2;");
    queries.push_back("select * from sbtest1 where k = 3 and c is not null;");

    for(const char* query : queries) {
        pair<float, float> res = connect_and_listen(program->getIP(), program->getPort(), &replicas, query);
        printf("%f,%f\n", res.first, res.second);
    }
    return 0;
}