#include "program.h"
#include <stdio.h>
#include <syslog.h>

Program* Program::singleton = 0;

Program::Program(){
    openlog("semantic-db", LOG_CONS | LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_LOCAL1);
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

