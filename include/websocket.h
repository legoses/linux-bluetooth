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
        bool thread = false;
        bool contFrag = false;
        bool bufSet = false;
        bool threadFree = false;

        //msg recieved from website
        uint8_t *msg;// = (uint8_t*)malloc(sizeof(this->maxPktSize)*sizeof(uint8_t));
        int maxPktSize;// = 2000;
        
        int msgSize;
        bool msgRecieved = false;

        //queue messages to send to client
        std::queue<char*> msgQueue;
        std::condition_variable listen_cv;
        std::mutex listen_mtx;

        //setup threads
        //when being initialized along with the class, curley brackets must be used
        //I think this is because it is abigious whether this is a function call
        //or a classs initializer, so the brackets differentiate between the two
        ThreadPool pool{2};

        //store command recieved
        uint8_t webAction = 0;


        int get_websocket_key(char *header, const int headerSize, unsigned char buffer[], int bufferSize);

        char *create_ws_header(char *buf, int size, int &hSize); 

        //parses data recieved
        int recv_data(char *buffer, int bufSize, uint8_t msg[], int msgSize); 
        uint8_t create_payload_indicator(bool frag);
        int create_frame(uint8_t buf[], char msg[], int msgLen, bool fragment);
        void print_frame(uint8_t frame[], int len);

    public:
        WebsocketServer(int port);
        ~WebsocketServer();

        void set_buffer(int size);

        void begin(); //loops infinantly so program will not exit after called
        int send_data(char msg[], int size, bool complete);

        void threaded_listener();
        void listener();

        void set_threading(bool opt);
        bool get_threading(); 

        int get_command(uint8_t *buf);
    };
};
#endif
