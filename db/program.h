#define MAX_LEN 30

class Program {
    public:
        Program();
        static Program* getInstance();
        void log(int priority, const char *format, ...);
        void setUp(int argc, char* argv[]);
        char* getConfigFilePath();
        
    private:
        static Program* singleton;
        int log_level = 5;
        char config_file_path[MAX_LEN] = "config";
        bool terminal_output = false;

        void usage();
};