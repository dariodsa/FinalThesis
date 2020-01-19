class Program {
    public:
        Program();
        static Program* getInstance();
        void log(int priority, const char *format, ...);
    private:
        static Program* singleton;
        int log_level = 5;
        bool terminal_output = false;
};