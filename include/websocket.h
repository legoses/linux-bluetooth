#ifndef SOCKET_TEST_H
#define SOCKET_TEST_H

#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>
#include "encode.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include "threadpool.h"


namespace Web {
    class WebsocketServer : public Encode {
        int listenSocket;
        sockaddr_in serverAddr;
        struct sockaddr clientAddr;
        int clientAddrSize;
        int clientSocket;
        int maxPktSize = 2000;
        bool thread = false;

        uint8_t &msg;

        //setup threads
        //when being initialized along with the class, curley brackets must be used
        //I think this is because it is abigious whether this is a function call
        //or a classs initializer, so the brackets differentiate between the two
        ThreadPool pool{2};

        //store command recieved
        uint8_t *webAction = 0;


        int get_websocket_key(char *header, const int headerSize, unsigned char buffer[], int bufferSize);

        char *create_ws_header(char *buf, int size, int &hSize); 

        //parses data recieved
        int recv_data(char *buffer, int bufSize, uint8_t msg[], int msgSize); 
        void create_frame(uint8_t buf[], char msg[], int msgLen);
        void print_frame(uint8_t frame[], int len);

    public:
        WebsocketServer(int port, uint8_t &msg_cb);
        ~WebsocketServer();

        void begin(); //loops infinantly so program will not exit after called
        void send_data(char msg[], int size);

        void threaded_listener();
        void listener();

        void set_threading(bool opt);
        bool get_threading(); 

        uint8_t *get_command();
    };
};
#endif
