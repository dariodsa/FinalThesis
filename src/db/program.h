#include <syslog.h>


#ifndef MAX_LEN
#define MAX_LEN 50
#endif

#ifndef PROGRAM_H
#define PROGRAM_H

class Program {
    public:
        Program();
        static Program* getInstance();
        void log(int priority, const char *format, ...);
        void setUp(int argc, char* argv[]);
        char* getConfigFilePath();
        int getPort();
        char* getIP();
        const char *data_location;

        static const signed int DEFAULT_CACHE_SIZE = 32*1024*1024;
        static const signed int AVERAGE_DATA_SIZE = 32;
        static constexpr float DEFAULT_TABLE_FETCH_SIZE = 0.20;

    private:
        static Program* singleton;
        int log_level = 5;
        int port = 7890;
        char IP[MAX_LEN];
        char config_file_path[MAX_LEN] = "config";
        bool terminal_output = true;

        void usage();
};

#endif