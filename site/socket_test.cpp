#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

int get_websocket_key(char header[], int headerSize, char buffer[], int bufferSize) {
    std::cout << "websocket key called\n";

    char searchStr[] = "Sec-WebSocket-Key: ";
    int searchSize = sizeof(searchStr); //counts null terminator. Subtract by 2 to offset
    int ret = -1; //return value

    for(int i = 0; i <= headerSize - searchSize; i++) {
        for(int j = 0; j <= searchSize; j++) {
            if(header[i+j] != searchStr[j]) {
                break;
            }

            if(j == searchSize-2) {
                std::cout << "websocket key found\n";
                ret = 0;
                int pos = i+j;
                int k = 1;

                //get length of key
                while(k+pos < headerSize && k < bufferSize) {
                    if(header[k+pos] == '\n') {
                        break;
                    }
                    k++; 
                } 
                //copy key from header to buffer usign key length and starting point
                memcpy(buffer, &header[pos+1], k*sizeof(char));

                return ret;
            }
        }
    }
    std::cout << "websocket key not found\n";
    return ret;
}

int main() {
    //steps:
    // 1. Listen for http connection from client
    // 2. On connection recieved, server must send back response that indicicates the protocol is changing from http to websocket
    //
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    int enable = 1;
    //allow program use reuse address
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        std::cout << "reuse addt failed\n";
    }
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0) {
        std::cout << "reuse addt failed\n";
    }


    //Soecify listening details
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    //serverAddress.sin_addr.s_addr = INADDR_LOOPBACK;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    int clientSocket = accept(serverSocket, nullptr, nullptr);

    char buffer[1024] = {0};
    char readBuffer[1024] = {0};
    char webkey[30];
    memset(webkey, '\0', sizeof(webkey));

    if(read(clientSocket, readBuffer, sizeof(readBuffer)) == 0) {
        std::cout << "Read does not wokr?\n";
    }

    std::cout << "\n\n\n";

    //get websocket key
    if(get_websocket_key(readBuffer, sizeof(readBuffer), webkey, sizeof(webkey)) == 0) {
        std::cout << "Webkey:\n";
        std::cout << webkey << "\n";
    }
    else {
        std::cout << "returned -1\n";
    }

    for(int i = 0; i < sizeof(readBuffer); i++) {
        std::cout << readBuffer[i];
    }

    recv(serverSocket, buffer, sizeof(buffer), 0);

    std::cout << "Client message:\n";
    for(int i = 0; i < sizeof(buffer); i++) {
        std::cout << buffer[i];
    }
    std::cout << "\n";

    close(serverSocket);
    return 0;
}

