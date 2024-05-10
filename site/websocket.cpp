#include "websocket.h"
//#include <websocket.h>

//create socket
Web::WebsocketServer::WebsocketServer(int port) {

    this->listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(this->listenSocket < 0) {
        std::cout << "Error opening socket\n";
    }
    int reuse = 1;

    //allow program use reuse address
    if(setsockopt(this->listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
        std::cout << "reuse addt failed\n";
    }
    if(setsockopt(this->listenSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(int)) < 0) {
        std::cout << "reuse port failed\n";
    }
    
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(8080);

    this->serverAddr.sin_addr.s_addr = INADDR_ANY;
    this->clientAddrSize = sizeof(clientAddr);
}

Web::WebsocketServer::~WebsocketServer() {
    close(this->listenSocket);
}


//begin connection to websocket server
//have this return some kind of value (bool, int) to signify value recieved
void Web::WebsocketServer::begin() {
    bind(this->listenSocket, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
    listen(this->listenSocket, 5);

    this->clientSocket = accept(this->listenSocket, &clientAddr, (socklen_t*)&clientAddrSize);

    const int bufSize = 1024;
    char *buffer = (char*)malloc(bufSize*sizeof(char));
    char *readBuffer = (char*)malloc(bufSize*sizeof(char));
      
    std::cout << "Reading input\n";
    std::cout << sizeof(readBuffer);
    if(read(clientSocket, readBuffer, bufSize) == 0) {
        std::cout << "Read does not wokr?\n";
    }

    int headerSize;
    char *wsHeader = create_ws_header(readBuffer, bufSize, headerSize);

    if(wsHeader != NULL) {
        send(this->clientSocket, wsHeader, headerSize-1, 0);
        memset(buffer, '\0', bufSize);
        recv(this->clientSocket, buffer, bufSize, 0);
        //read(clientSocket, buffer)
       
        //uint8_t *connBuf = (uint8_t*)malloc(bufSize*sizeof(uint8_t));
        //detect if websocket connection was successful
        if(buffer[0] != 0) {
            std::cout << "Websocket Connection Successful\n";
            uint8_t msg[bufSize];
            memset(msg, '\0', bufSize);

            listener();
        }
        else {
            std::cout << "Error: Connection failed\n";
        }
    }

    free(buffer);
    free(readBuffer);
    free(wsHeader);

}

int Web::WebsocketServer::get_websocket_key(char *header, const int headerSize, unsigned char buffer[], int bufferSize) {
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


char *Web::WebsocketServer::create_ws_header(char *buf, int size, int &hSize) {
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


int Web::WebsocketServer::recv_data(char *buffer, int bufSize, uint8_t msg[], int msgSize) {
    std::cout << "Data recieved\n";
    //check if this contains a message, and is final packet in stream
    int len;
    if((((buffer[0]&0b10000001) == 0b10000001)) && (((unsigned char)buffer[1] >> 7) == 1)) {
        std::cout << "129 detected\n";

        //get length of message
        int lenIndic = (unsigned char)buffer[1]-128;
       
        //Hold th ekey and encoded date. Decoded data will be stored in msg param
        uint8_t key[4];
        uint8_t *encoded;

        if(lenIndic <= 125 && lenIndic >= 0) {
            memcpy(key, &buffer[2], 4);
            len = lenIndic;
           
            if(lenIndic < 255) {
                encoded = (uint8_t*)malloc(len*sizeof(uint8_t));
                memcpy(encoded, &buffer[6], len);
            }


        }
        else if(lenIndic == 126) {
            memcpy(key, &buffer[4], 4);
            len = (unsigned char)buffer[2] << 8 | (unsigned char)buffer[3];
           
            //make sure len is not larger than msg buffer
            if(len < msgSize) {
                encoded = (uint8_t*)malloc(len*sizeof(uint8_t));
                memcpy(encoded, &buffer[8], len);
            }
        }
        else if(lenIndic == 127) { //MSB must be 0. Will most likely not be called
            std::cout << "Recieved Message too large\n";
            return -1;
        }
        else {
            std::cout << "Unable to decode message\n";
            return -1;
        }

        for(int i = 0; i < len; i++) {
            //decode message
            msg[i] = (encoded[i] ^ key[i & 0x3]);
        }
        free(encoded);
        std::cout << "Length: " << len << "\n";
    }
    else {
        std::cout << "No 129\n";
    }
    std::cout << "Length 2: " << len << "\n";

    if(len < maxPktSize) {
        return len;
    }
    return -2;
}


//create frame before sending to client
void Web::WebsocketServer::create_frame(uint8_t buf[], char msg[], int msgLen) {
    //add check for packet size later
    if(msgLen < 126) {
        //uint8_t header[this->maxPktSize];
        //snprintf(buf, 200, "%d%d%d\r\n\r\n", 129, msgLen, msg);
        buf[0] = 129; //indicate this is the final frame, and that it contains text
        buf[1] = msgLen; //set mask bit to 0, and indicate message length
        memcpy(&buf[2], msg, msgLen); //copy message to buffer
        memcpy(&buf[2+msgLen], "\r\n\r\n", 4); //end the packet
        std::cout << "Frame created\n";

    }
}


//send packet to client
void Web::WebsocketServer::send_data(char msg[], int size) {
    
    uint8_t packet[this->maxPktSize];
    create_frame(packet, msg, size);
    send(this->clientSocket, msg, size+2, 0);
}

//listen for incoming messages
int Web::WebsocketServer::listener() {
    std::cout << "Listening for messages...\n";
    int bufSize = 1024;
    char *buffer = (char*)malloc(bufSize*sizeof(char));
    uint8_t msg[bufSize];
    memset(msg, '\0', bufSize);
    memset(buffer, '\0', bufSize);

    while(true) {
        //reset buffers

        recv(this->clientSocket, buffer, bufSize, 0);
        if(buffer[0] != '\0') {
            int size = recv_data(buffer, bufSize, msg, bufSize);
            if(size > 0) {
                func_cb(msg, size);
            }
            memset(msg, '\0', bufSize);
            memset(buffer, '\0', bufSize);
        }
    }
}


void Web::WebsocketServer::set_cb(void (*funcptr)(uint8_t[], int)) {
   this->func_cb = funcptr; 
}


