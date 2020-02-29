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


Proxy::Proxy(int port) {
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
        processQuery(code, sql);
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

void Proxy::processQuery(char type, char* query) {
    //send_row_descriptor
    //send_row_data
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