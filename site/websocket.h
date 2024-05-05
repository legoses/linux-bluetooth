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
    int listenSocket;
    sockaddr_in serverAddr;
    struct sockaddr clientAddr;
    int clientAddrSize;
    int clientSocket;

    int get_websocket_key(char *header, const int headerSize, unsigned char buffer[], int bufferSize);

    char *create_ws_header(char *buf, int size, int &hSize); 

    int recv_data(char *buffer, int bufSize, uint8_t msg[], int msgSize); 

    public:
    WebsocketServer(int port);
    ~WebsocketServer();


    void begin();
};
#endif
