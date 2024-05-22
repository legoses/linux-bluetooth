#ifndef SOCKET_TEST_H
#define SOCKET_TEST_H

#include <cstring>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>
#include "encode.h"
#include <iostream>
#include <future>


namespace Web {
    class WebsocketServer : public Encode {
        int listenSocket;
        sockaddr_in serverAddr;
        struct sockaddr clientAddr;
        int clientAddrSize;
        int clientSocket;
        int maxPktSize = 2000;
        void (*func_cb)(uint8_t[], int);
        int cbSet = 0;

        int get_websocket_key(char *header, const int headerSize, unsigned char buffer[], int bufferSize);

        char *create_ws_header(char *buf, int size, int &hSize); 

        //parses data recieved
        int recv_data(char *buffer, int bufSize, uint8_t msg[], int msgSize); 
        void create_frame(uint8_t buf[], char msg[], int msgLen);
        void print_frame(uint8_t frame[], int len);

        public:
        WebsocketServer(int port);
        ~WebsocketServer();

        void begin(); //loops infinantly so program will not exit after called
        void send_data(char msg[], int size);
        //int listener(uint8_t buf[], int bufSize);
        int listener();
        void set_cb(void (*funcptr)(uint8_t[], int));
    };
};
#endif
