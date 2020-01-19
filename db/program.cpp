#include "program.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>

Program* Program::singleton = 0;

Program::Program(){
    openlog("semantic-db", LOG_CONS | LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_LOCAL1);
}

void Program::setUp(int argc, char* argv[]) {
    int opt;
    while( (opt = getopt(argc, argv, "c:l:")) != -1) {
        
        switch(opt) {
            case 'c':
                strcpy(config_file_path, optarg);
                break;
            case 'l':
                log_level = atoi(optarg);
                break;
            default:
                printf("Parameter %s is not acceptable.\n", optarg);
                //usage();
                return;
        }
    }

}

char* Program::getConfigFilePath() {
    return this->config_file_path;
}

void Program::log(int priority, const char *format, ...) {
    if( log_level >= priority ) {
        syslog(priority, format);
        if(terminal_output) {
            fprintf(stderr, format);
        }
    }
    return;
}

Program* Program::getInstance() {
    if(singleton == 0) {
        singleton = new Program();
    }
    return singleton;
}

void Program::usage() {

}
