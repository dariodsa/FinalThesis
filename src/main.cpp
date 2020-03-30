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
    queries.push_back("SELECT d.dashboardid,d.name,d.userid FROM dashboard d, graphs_items gi;");

    for(const char* query : queries) {
        pair<float, float> res = connect_and_listen(program->getIP(), program->getPort(), &replicas, query);
    }
    return 0;
}