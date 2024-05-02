#include <cstring>
#include <unistd.h>
#include <string>
#include <openssl/sha.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>
#include "encodeTest.cpp"


/*
 * TODO:
 * Handle incoming data from websocket, possible via function callback
 */


int get_websocket_key(char *header, const int headerSize, unsigned char buffer[], int bufferSize) {
    std::cout << "websocket key called\n";

    char searchStr[] = "Sec-WebSocket-Key: ";
    int searchSize = sizeof(searchStr); //counts null terminator. Subtract by 2 to offset
    int ret = -1; //return value
    std::cout << searchSize << "\n";

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
                
                unsigned char uuid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

                memcpy(buffer, &header[pos+1], 24*sizeof(char));
                memcpy(&buffer[24], uuid, 36*sizeof(char)); 
                
                return ret;
            }
        }
    }
    std::cout << "websocket key not found\n";
    return ret;
}


char *create_ws_header(char *buf, int size, int &hSize) {
    char initReg[] = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
    uint8_t webkey[60];
    memset(webkey, '\0', sizeof(webkey));
    char *wsHeader;

    if(get_websocket_key(buf, size, webkey, sizeof(webkey)) == 0) {
        int baseSize = base64_length(SHA_DIGEST_LENGTH)+4; //add 4 to include space for \r\n\r\n packet ender
        
        uint8_t base64[baseSize];
        int hashSize = SHA_DIGEST_LENGTH;
        uint8_t *hashBuf = (uint8_t*)malloc(hashSize*sizeof(uint8_t));

        gen_sha_hash(webkey, sizeof(webkey), hashBuf);

        gen_base64(hashBuf, hashSize, base64, baseSize-4);
        
        memcpy(&base64[baseSize-4], "\r\n\r\n", 4);

        int headerSize = sizeof(initReg) + baseSize;
        if(headerSize < 1024) {
            wsHeader = (char*)malloc(headerSize*sizeof(char));
            hSize = headerSize;
            //copy header and base64 code into string
            memcpy(wsHeader, initReg, sizeof(initReg));
        
            if(headerSize - sizeof(initReg) >= baseSize) {
                std::cout << "copying text to: " << sizeof(initReg) << "\n";
                memcpy(&wsHeader[sizeof(initReg)-1], base64, (baseSize*sizeof(uint8_t)));
            }
            
            return wsHeader;
        }
        else {
            std::cout << "Size of header too large\n";
        }
    }
    wsHeader = NULL;
    return wsHeader;
}


void recv_data(char *buffer, int bufSize) {
    //std::cout << "bytes test:\n";
    //std::cout << (buffer[0]&0b10000001) << "\n";
    if((buffer[0]&0b10000001) == 0b10000001) {
        std::cout << "129 detected\n";
    }
    else {
        std::cout << "No 129\n";
    }
    std::cout << "first bit: " << (buffer[0] >> 7) << "\n";
    for(int i = 0; i < bufSize; i++) {
        //std::cout << (buffer[i] | 0b00000000) << " ";
        printf("%02x ", buffer[i]);
    }
    std::cout << "\n";
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
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    int clientSocket = accept(serverSocket, &clientAddr, (socklen_t*)&clientAddrSize);

    const int bufSize = 1024;
    char *buffer = (char*)malloc(bufSize*sizeof(char));
    char *readBuffer = (char*)malloc(bufSize*sizeof(char));
   
    std::cout << "Reading input\n";
    std::cout << sizeof(readBuffer);
    if(read(clientSocket, readBuffer, bufSize) == 0) {
        std::cout << "Read does not wokr?\n";
    }

    /*------------create ws header----------*/
    int headerSize;
    char *wsHeader = create_ws_header(readBuffer, bufSize, headerSize);

    if(wsHeader != NULL) {
        send(clientSocket, wsHeader, headerSize-1, 0);
        memset(buffer, '\0', bufSize);
        recv(clientSocket, buffer, bufSize, 0);
        //read(clientSocket, buffer)
       
        uint8_t *connBuf = (uint8_t*)malloc(bufSize*sizeof(uint8_t));
        //detect if websocket connection was successful
        if(buffer[0] != 0) {
            std::cout << "Websocket Connection Successful\n";
            while(true) {
                recv_data(buffer, bufSize);
                memset(connBuf, '\0', bufSize);
                recv(clientSocket, connBuf, bufSize, 0);

                sleep(1);
            }
            free(connBuf);
        }
        else {
            std::cout << "Error: Connection failed\n";
        }
    }

    free(buffer);
    free(readBuffer);
    close(serverSocket);
    free(wsHeader);
    return 0;
}

