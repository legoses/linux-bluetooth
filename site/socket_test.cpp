#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <string>
#include <openssl/sha.h>
#include <stdint.h>


char constexpr encode_table[] {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
    '3', '4', '5', '6', '7', '8', '9', '+', '/'};


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


void gen_base64(char *digest, int digestSize, uint8_t *base64) {
    int base64i = 0;
    int i = 0;
    
    std::cout << "Converting to base64\n";
    //use 3 bytes at a time to convert into base64
    while(i < digestSize) {
        uint32_t bits = (digest[i] << 16) | (digest[i+1] << 8) | (digest[i+2]);
        base64[base64i] = encode_table[(bits >> 18) & 0b0011'1111];
        base64[base64i+1] = encode_table[(bits >> 12) & 0b0011'1111];
        base64[base64i+2] = encode_table[(bits >> 6) & 0b0011'1111];
        base64[base64i+3] = encode_table[bits & 0b0011'1111];
        base64i+=4;
        i+=3;
    }

    //convert the remaining 2 bits
    if(digestSize-(i-2) == 1) {
        i-=3;
        base64-=4;
        uint32_t bits = (digest[i] << 16) | 0x00 << 8 | 0x00;
        
        base64[base64i] = encode_table[(bits >> 18) & 0b0011'1111];
        base64[base64i+1] = encode_table[(bits >> 12) & 0b0011'1111];
        base64[base64i+2] = '=';
        base64[base64i+3] = '=';
    }
}


void gen_sha_hash(const unsigned char input[], int inputSize, char *hashBuf)  {
    unsigned char digest[SHA_DIGEST_LENGTH] = {0};
    unsigned char *sha = SHA1(input, inputSize, digest);

    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(&hashBuf[i*2], "%02x", (unsigned int)digest[i]);
    }

}


int upgrade_to_ws(char *readBuffer, const int bufSize, int conn) {
    std::cout << "Upgrading connection\n";
    unsigned char webkey[60];
    memset(webkey, '\0', sizeof(webkey));

    char initReg[] = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";

    if(get_websocket_key(readBuffer, bufSize, webkey, sizeof(webkey)) == 0) {
        std::cout << webkey << "\n";
        //allocate 2 more than needed forr \r\n return characters
        int baseSize = 60; //figure out a way to calculate this instead of hard coding
        uint8_t base64[baseSize];
        int hashSize = SHA_DIGEST_LENGTH*2+1;
        char *hashBuf = (char*)malloc(hashSize*sizeof(char));

        gen_sha_hash(webkey, sizeof(webkey), hashBuf);
        std::cout << "Hash buf: " << hashBuf << "\n";
        //gen_base64(hashBuf, sizeof(hashBuf)/sizeof(char), base64);
        gen_base64(hashBuf, hashSize, base64);
        memcpy(&base64[baseSize-4], "\r\n\r\n", 4);
        

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

        //send header to client to complete upgrade
        send(conn, wsHeader, headerSize-2, 0);

        free(wsHeader);

        //make sure to send to client before freeing memory
        free(hashBuf);
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

    std::cout << "Client addr: " << clientAddr.sa_data << "\n";

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

    }

    std::cout << "\n";

    free(buffer);
    free(readBuffer);
    close(serverSocket);
    return 0;
}

