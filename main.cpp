#include <stdio.h>
#include "db/config.h"
#include <libpq-fe.h>
#include <vector>

std::vector<Database*> replicas;

int log_level;

int main(int argc, char* argv[]) {
    
    Program* program = Program::getInstance();
    program->setUp(argc, argv);

    char *CONFIG_FILE_PATH = program->getConfigFilePath();

    replicas = setup_db_replicas_pool(CONFIG_FILE_PATH);
    
    if(replicas.size() == 0) {
        program->log(LOG_WARNING, "Number of databases are zero.\n");
        exit(0);
    } 
    
    connect_to_replicas(replicas);


    return 0;
}