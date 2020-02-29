class Proxy {
    public:
        Proxy(int port);
    
    private:
        void read_ssl(int conn);
        void send_notice(int conn);
        void read_startup_message(int conn);
        void send_auth_request(int conn);
        void read_auth(int conn);
        void send_auth_ok(int conn);
        void send_ready_for_query(int conn);

        void processQuery(char type, char* query);

        bool readSock(int conn, char *pointer, int len);

        int port;
};