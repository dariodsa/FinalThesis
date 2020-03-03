#include "proxy.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>


Proxy::Proxy(int port, Database* d1) {
    int s1 = socket(AF_INET, SOCK_STREAM, 0);
    
    unsigned int len; 
    struct sockaddr_in cli; 
    
    struct sockaddr_in my_addr;
    
    my_addr.sin_family = AF_INET;    
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    bind(s1, (struct sockaddr *)&my_addr, sizeof my_addr);

    listen(s1, 10);

    int conn = accept(s1, (sockaddr*)&cli, &len); 

    read_ssl(conn);
    send_notice(conn);
    read_startup_message(conn);
    send_auth_request(conn);
    read_auth(conn);
    send_auth_ok(conn);
    while(true) {
        send_ready_for_query(conn);
        char code;
        read(conn, &code, 1);
        unsigned int msglen = 0; // 4 bytes
        read(conn, &msglen, 4);

        msglen = ntohl(msglen);

        char* sql = (char*) malloc(sizeof(char) * (msglen - 3));
        read(conn, sql, msglen - 4);
        sql[msglen-3] = 0;
        printf("%c %s\n", code, sql);
        processQuery(d1, conn, sql);
    }
}

bool Proxy::readSock(int s1, char* arr, int len) {
    int l1 = 0;
    int l2 = len;
    while(l2 > 0) {
        l1 = read(s1, arr, l2);
        if(l1 == -1) return false;
        l2 = l2 - l1;
        arr = arr + l1;
    }
    return true;
}

void Proxy::send_row_descriptor(PGresult* res, int s1, int col_count) {
    int size = 0;
    
    for(int col = 0; col < col_count; ++col) {
        char* col_name = PQfname(res, col);
        size += strlen(col_name) + 1 + 4 + 2 + 4 + 2 + 4 + 2;
    }
    char t = 'T';
    int k = write(s1, &t, 1);
    size += 6;
    
    size = htonl(size);
    k = write(s1, &size, 4);
    
    short c1 = col_count;
    c1 = htons(c1);
    
    k = write(s1, &c1, 2);
    
    for(int col = 0; col < col_count; ++col) {
        char* col_name = PQfname(res, col);
    
        write(s1, col_name, strlen(col_name));
        char n = NULL;
        write(s1, &n, 1);
        int br = htonl(0);
        short a = htons(0);
        
        write(s1, &br, 4); //table id
        write(s1, &a, 2); //column id
        //23 4
        
        br = htonl(23);//PQparamtype(res, col);
        a = htons(4);//PQfsize(res, col);
        write(s1, &br, 4); //type id
        write(s1, &a, 2); //type size
        
        br = htonl(-1);
        a = htons(0);
        write(s1, &br, 4); //type modifier
        write(s1, &a, 2); //text format code
        
    }
}

void Proxy::send_row_data(PGresult* res, int s1, int row_count, int col_count) {
    for(int row = 0; row < row_count; ++row) {
        int size = 0;
        for(int col = 0; col < col_count; ++col) {
            char *row_data = PQgetvalue(res, row, col);
            
            int len = strlen(row_data);
            size += 4 + len;
        }
        char d = 'D';
        write(s1, &d, 1);
        int l1 = 6 + size;
        
        l1 = htonl(l1);
        write(s1, &l1, 4);
        short a = col_count;
        a = htons(a);
        write(s1, &a, 2);
        for(int col = 0; col < col_count; ++col) {
            char *row_data = PQgetvalue(res, row, col);
            
            int len = strlen(row_data);
            int len1 = htonl(len);
            write(s1, &len1, 4);
            write(s1, row_data, len);
            char n = NULL;
            
        }
    }
}

void Proxy::processQuery(Database* d1, int s1, char* query) {
    PGresult* res = d1->executeQuery(query);
    
    int rec_count = PQntuples(res);
    int col_count = PQnfields(res);

    //printf("We have %d rows.\n", rec_count);
    /*for(int row = 0; row < rec_count; ++row) {
        for(int col = 0; col < col_count; ++col) {
            printf("%s ", PQgetvalue(res, row, col));
        }
        printf("\n");
    }*/
    
    send_row_descriptor(res, s1, col_count);
    send_row_data(res, s1, rec_count, col_count);

    char c = 'C';
    write(s1, &c, sizeof(char));
    int l = strlen("SELECT") + 4 + 1;
    l = htonl(l);
    write(s1, &l, sizeof(int));
    char *select = (char*)malloc(7);
    select[6] = 0;
    strcpy(select, "SELECT");
    
    int k = write(s1, select, 7);
    
}

void Proxy::send_ready_for_query(int conn) {
    char *data = (char*) malloc(6);
    memset(data, 0, 6);
    data[0] = 'Z';
    data[4] = 5;
    data[5] = 'I';
    write(conn, data, 6);
}

void Proxy::send_auth_ok(int conn) {
    char *data = (char*) malloc(9);
    memset(data, 0, 9);
    data[0] = 'R';
    data[4] = 8;
    write(conn, data, 9);
}

void Proxy::read_auth(int conn) {
    char type;
    read(conn, &type, 1);
    printf("%d %c\n", type, type);
    unsigned int msglen;
    read(conn, &msglen, 4);
    msglen = ntohl(msglen);
    printf("paass len %d\n", msglen);
    char* password = (char*) malloc(sizeof(char) * (msglen - 3));
    read(conn, password, msglen - 4);
    //password[msglen - 3] = 0;
    printf("Pass %s\n", password);
}

void Proxy::send_auth_request(int conn) {
    char *data = (char*) malloc(9);
    memset(data, 0, 9);
    data[0] = 'R';
    data[4] = 8;
    data[8] = 3;
    write(conn, data, 9);
}

void Proxy::read_startup_message(int conn) {
    int msglen, version;
    read(conn, &msglen, 4);
    read(conn, &version, 4);
    msglen = ntohl(msglen);
    version = ntohl(version);
    printf("%d %d\n", msglen, version);
    char *data = (char*) malloc(sizeof(char) * (msglen - 8));
    read(conn, data, msglen - 8);
    printf("Startup mess:\n");
    for(int i = 0; i < msglen - 9; ++i) printf("%c", data[i]);
    printf("\n");
}

void Proxy::send_notice(int conn) {
    char n = 'N';
    write(conn, &n, 1);
}
void Proxy::read_ssl(int conn)  {
    int msglen, sslcode;
    read(conn, &msglen, 4);
    read(conn, &sslcode, 4);
    msglen = ntohl(msglen);
    sslcode = ntohl(sslcode);
    printf("%d %d\n", msglen, sslcode);
    if(msglen != 8 && sslcode != 80877103) {}
}