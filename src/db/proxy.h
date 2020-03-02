#include "../structures/database.h"

class Proxy {
    public:
        Proxy(int port, Database* d1);
    
    private:
        void read_ssl(int conn);
        void send_notice(int conn);
        void read_startup_message(int conn);
        void send_auth_request(int conn);
        void read_auth(int conn);
        void send_auth_ok(int conn);
        void send_ready_for_query(int conn);

        void processQuery(Database *d, int s1, char* query);
        void send_row_data(PGresult* res, int s1, int row_count, int col_count);
        void send_row_descriptor(PGresult* res, int s1, int col_count);

        bool readSock(int conn, char *pointer, int len);

        int port;
};