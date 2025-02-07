#include "program.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <stdarg.h>

Program* Program::singleton = NULL;
char Program::name[5*MAX_LEN];

Program::Program(const char* name){
    
}

void Program::setUp(int argc, char* argv[]) {
    int opt;
    while( (opt = getopt(argc, argv, "c:l:p:")) != -1) {
        switch(opt) {
            case 'c':
                strcpy(config_file_path, optarg);
                break;
            case 'l':
                log_level = atoi(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                printf("Parameter %s is not acceptable.\n", optarg);
                //usage();
                return;
        }
    }
    return;
}

char* Program::getConfigFilePath() {
    return this->config_file_path;
}

void Program::log(int priority, const char *format, ...) {
    
    openlog(Program::name, LOG_CONS | LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_LOCAL1);
    if( log_level >= priority ) {
        va_list list;
        va_start(list, format);
        vsyslog(priority, format, list);
        if(!terminal_output) {
            vfprintf(stderr, format, list);
        }
        va_end(list);
    }
    closelog();
    return;
}

Program* Program::getInstance() {
    if(singleton == NULL) {
        singleton = new Program(name);
    }
    return singleton;
}

int Program::getPort() {
    return this->port;
}

char* Program::getIP() {
    return this->IP;
}

void Program::usage() {

}

void Program::setName(char *name) {
    strcpy(Program::name, name);
}