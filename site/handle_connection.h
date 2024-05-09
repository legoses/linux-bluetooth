#ifndef HANDLE_CONNECTION_H
#define HANDLE_CONNECTION_H

//this class will handle overall socket connection
class ConnHandler {
    static int listenSocket;
    sockaddr_in serverAddr;
    //struct sockaddr clientAddr;
    //int clientAddrSize;
    //int clientSocket;
    int maxPktSize = 2000;
    public:

    void create_socket() {
        
    }
        
};

#endif
