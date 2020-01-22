#include <stdio.h>
#include "db/config.h"
#include <libpq-fe.h>
#include <vector>

// list of replicas involved
std::vector<Database*> replicas;


pthread_mutex_t replicas_lock;

/*
 * thread which is responsible for contacting replicas 
 * and for reporting about their life
*/
pthread_t keep_alive_thread;


int main(int argc, char* argv[]) {
    
    Program* program = Program::getInstance();
    
    program->setUp(argc, argv);

    char *CONFIG_FILE_PATH = program->getConfigFilePath();

    replicas = setup_db_replicas_pool(CONFIG_FILE_PATH);
    
    if(replicas.size() == 0) {
        program->log(LOG_WARNING, "Number of databases are zero.\n");
        exit(0);
    } 
    
    if( !connect_to_replicas(replicas) ) {
        return 0;
    }
    
    bool value = connect_and_listen(program->getIP(), program->getPort());

    return 0;
}