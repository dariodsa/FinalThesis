#include <syslog.h>


#ifndef MAX_LEN
#define MAX_LEN 50
#endif

#ifndef PROGRAM_H
#define PROGRAM_H

class Program {
    public:
        Program(const char* name);
        static Program* getInstance();
        static void setName(char* name);
        void log(int priority, const char *format, ...);
        void setUp(int argc, char* argv[]);
        char* getConfigFilePath();
        int getPort();
        char* getIP();
        const char *data_location;

        static const signed int AVERAGE_DATA_SIZE = 16;

    private:
        static Program* singleton;
        int log_level = 5;
        int port = 7890;
        char IP[MAX_LEN];
        char config_file_path[MAX_LEN] = "config";
        bool terminal_output = true;

        static char name[5*MAX_LEN];

        void usage();
};

#endif