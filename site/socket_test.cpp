#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <string>
#include <openssl/sha.h>
#include <stdint.h>
#include "encodeTest.cpp"

/*
 * TODO:
 * Encode text to utf-8 before upgrading to websocket
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


int upgrade_to_ws(char *readBuffer, const int bufSize, int conn) {
    std::cout << "Upgrading connection\n";
    uint8_t webkey[60];
    memset(webkey, '\0', sizeof(webkey));

    char initReg[] = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";

    if(get_websocket_key(readBuffer, bufSize, webkey, sizeof(webkey)) == 0) {
        std::cout << webkey << "\n";
        //allocate 2 more than needed forr \r\n return characters
        //int baseSize = 60; //figure out a way to calculate this instead of hard coding
        int baseSize = base64_length(20) + 4; //add 4 to include space for \r\n\r\n packet ender
        
        uint8_t base64[baseSize];
        //int hashSize = SHA_DIGEST_LENGTH*2+1;
        int hashSize = SHA_DIGEST_LENGTH;
        uint8_t *hashBuf = (uint8_t*)malloc(hashSize*sizeof(uint8_t));

        gen_sha_hash(webkey, sizeof(webkey)-1, hashBuf);

        std::cout << "Hash buf: " << hashBuf << "\n";
        std::cout << "post base test: " << baseSize << "\n";
        gen_base64(hashBuf, hashSize, base64, baseSize-4);
        
        std::cout << "memcpy: " << baseSize << "\n";
        memcpy(&base64[baseSize-4], "\r\n\r\n", 4);
        std::cout << "post memcpy:\n";

        int headerSize = sizeof(initReg) + baseSize;
        char *wsHeader = (char*)malloc(headerSize*sizeof(char));
        //copy header and base64 code into string
        memcpy(wsHeader, initReg, sizeof(initReg));
        //make sure there is enough room to store base64
        std::cout << "Base64: " << base64 << "\n";
        if(headerSize - sizeof(initReg) >= baseSize) {
            std::cout << "copying text to: " << sizeof(initReg) << "\n";
            memcpy(&wsHeader[sizeof(initReg)-1], &base64, (baseSize*sizeof(uint8_t))-1);
        }

        int utfSize = calc_utf_size(headerSize);
        char *wsUTF8 = (char*)malloc(utfSize);

        char_to_utf8(wsUTF8, utfSize, &wsHeader, headerSize);


        //send header to client to complete upgrade
        send(conn, wsHeader, headerSize-2, 0);

        free(wsHeader);

        //make sure to send to client before freeing memory
        free(hashBuf);
        free(wsUTF8);
        return 0;
    }
    return -1;
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
    std::cout << "print test: \n";
    std::cout << readBuffer << "\n";

    //get websocket key
    //upgrade http connection to websocket
    if(upgrade_to_ws(readBuffer, bufSize, clientSocket) == 0) {
        recv(serverSocket, buffer, sizeof(buffer), 0);
        std::cout << "Socket upgraded\n";
    }

    while(true) {
        free(buffer);
        buffer = (char*)malloc(bufSize*sizeof(char));
        recv(serverSocket, buffer, bufSize, 0);
        std::cout << "Message from client:\n";
        for(int i = 0; i < bufSize; i++) {
            std::cout << (uint8_t)buffer[i];
        }
        std::cout << "\n";
        break;

    }
    std::cout << "\n";

    free(buffer);
    free(readBuffer);
    close(serverSocket);
    return 0;
}

