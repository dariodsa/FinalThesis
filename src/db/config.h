#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <syslog.h>
#include "../structures/database.h"
//#include "../data/y.tab.h"
extern "C"{
#include "../data/y.tab.c"
#include "../data/lex.yy.c"
}
#include "program.h"



#define ARG_NUMBERS 2
#define MAX_PORT 65535

bool connect_and_listen(char *ip, int port) {
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
    FILE *yyin = fopen("../data/i1", "r");
    printf("%ld\n", yyin);
    
    printf("Call parse:\n");
    parse(yyin);
    printf("Done parse:\n");
    
    

    return true;
}

bool connect_to_replicas(std::vector<Database*> replicas) {
    Program *program = Program::getInstance();

    for(Database* replica : replicas) {
        if( !replica->connect() ) {
            return false;
        }
    }
    return true;
}

std::vector<Database*> setup_db_replicas_pool(char *file_path) {

    Program *program = Program::getInstance();

    std::vector<Database*> replicas;
    FILE *config_file = fopen(file_path, "r");
    if(config_file == NULL) {
        program->log(LOG_ALERT, "Config file doesn't exsist.\n");
        exit(1);
    }
    
    char _ip[MAX_LEN];
    int _port;
    int line_num = 1;
    int arg;
    while( ( arg = fscanf(config_file, "%[^,],%d\n", &_ip, &_port)) != EOF) {
        
        if(arg != ARG_NUMBERS) {
            program->log(LOG_ALERT, "Error in reading the config file[%s] in line %d.\n%m\n", file_path, line_num);
            exit(1);
        }

        program->log(LOG_INFO, "Adding database info: IP: %s,  port:%d", _ip, _port);
        replicas.push_back(new Database(_ip, _port));
        
        line_num += 1;
    }

    fclose(config_file);
    closelog();
    return replicas;
 }