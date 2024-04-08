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


int get_websocket_key(char header[], int headerSize, unsigned char buffer[], int bufferSize) {
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


uint8_t *gen_base64(char digest[], int digestSize) {
    uint8_t *base64 = (uint8_t*)malloc(60*sizeof(uint8_t));
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

    return base64;
}


void gen_sha_hash(const unsigned char input[], int inputSize, char hashBuf[])  {
    unsigned char digest[SHA_DIGEST_LENGTH] = {0};
    unsigned char *sha = SHA1(input, inputSize, digest);

    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(&hashBuf[i*2], "%02x", (unsigned int)digest[i]);
    }

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
    unsigned char webkey[60];
    memset(webkey, '\0', sizeof(webkey));

    if(read(clientSocket, readBuffer, sizeof(readBuffer)) == 0) {
        std::cout << "Read does not wokr?\n";
    }

    std::cout << "\n\n\n";
    //get websocket key
    if(get_websocket_key(readBuffer, sizeof(readBuffer), webkey, sizeof(webkey)) == 0) {
        char hashBuf[SHA_DIGEST_LENGTH*2 + 1];

        gen_sha_hash(webkey, sizeof(webkey), hashBuf);
        uint8_t *base64 = gen_base64(hashBuf, sizeof(hashBuf)/sizeof(char));
        std::cout << sizeof(hashBuf) << "\n";

        //make sure to send to client before freeing memory
        free(base64);
        recv(serverSocket, buffer, sizeof(buffer), 0);



        std::cout << "\n";
    }

    close(serverSocket);
    return 0;
}

