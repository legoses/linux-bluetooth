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

class WebsocketServer : public Encode {
    const char *ipAddr;
    const int port;
    int listenSocket;

    public:
    WebsocketServer(char addr[], int port);
    ~WebsocketServer();

    int get_websocket_key(char *header, const int headerSize, unsigned char buffer[], int bufferSize);

    char *create_ws_header(char *buf, int size, int &hSize); 

    int recv_data(char *buffer, int bufSize, uint8_t msg[], int msgSize); 
};

#endif
