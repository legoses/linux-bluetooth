#include <websocket.h>
//#include <websocket.h>

//create socket
Web::WebsocketServer::WebsocketServer(int port)
{

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
    
    //this->webAction = (uint8_t*)malloc(sizeof(uint8_t));
}


Web::WebsocketServer::~WebsocketServer() {
    close(this->listenSocket);
}


void Web::WebsocketServer::print_frame(uint8_t frame[], int len) {
    std::cout << "Printing frame:\n\n";
    for(int i = 0; i < len; i++) {
        std::cout << (int)frame[i] << " ";
    }
    std::cout << "\n\n";
}


//begin connection to websocket server
//have this return some kind of value (bool, int) to signify value recieved
void Web::WebsocketServer::begin() {
    std::cout << "Listening for connections...\n";
    //create socket
    bind(this->listenSocket, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
    listen(this->listenSocket, 5);

    //listen for connections
    this->clientSocket = accept(this->listenSocket, &clientAddr, (socklen_t*)&clientAddrSize);

    char *buffer = (char*)malloc(this->maxPktSize*sizeof(char));
    char *readBuffer = (char*)malloc(this->maxPktSize*sizeof(char));
      
    std::cout << "Reading input\n";
    std::cout << sizeof(readBuffer);
    if(read(this->clientSocket, readBuffer, this->maxPktSize) == 0) {
        std::cout << "Read does not wokr?\n";
    }

    int headerSize;
    char *wsHeader = create_ws_header(readBuffer, this->maxPktSize, headerSize);

    if(wsHeader != NULL) {
        std::cout << "Init WS upgrade\n";
        memset(buffer, '\0', this->maxPktSize);
        send(this->clientSocket, wsHeader, headerSize-1, 0);
        recv(this->clientSocket, buffer, this->maxPktSize, 0);

        //use ping/pong frame to test connection is alive
        //Check if connection has been terminated
        //read(this->listenSocket, readBuffer, this->maxPktSize);
        //print_frame(readBuffer, this->maxPktSize);

       
        //uint8_t *connBuf = (uint8_t*)malloc(this->maxPktSize*sizeof(uint8_t));
        //detect if websocket connection was successful
        //listener();
        
        if(buffer[0] != 0) {
            std::cout << "Websocket Connection Successful\n";
            uint8_t msg[this->maxPktSize];
            memset(msg, '\0', this->maxPktSize);
            
            if(this->thread == false) {
                listener();
            }
            else {
                threaded_listener();

                std::cout << "Post threaded listener call\n";
            }
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
    uint8_t webkey[60];
    memset(webkey, '\0', sizeof(webkey));
    char *wsHeader;

    if(get_websocket_key(buf, size, webkey, sizeof(webkey)) == 0) {
        char initReg[] = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
        int baseSize = base64_length(SHA_DIGEST_LENGTH)+4; //add 4 to include space for \r\n\r\n packet ender
        
        uint8_t base64[baseSize];
        int hashSize = SHA_DIGEST_LENGTH;
        uint8_t *hashBuf = (uint8_t*)malloc(hashSize*sizeof(uint8_t));

        gen_sha_hash(webkey, sizeof(webkey), hashBuf);
        gen_base64(hashBuf, hashSize, base64, baseSize-4);
       
        //indicate end of packet
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


//convert websocket data into readable format
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

        if(lenIndic <= 125 && lenIndic >= 0) { //Check if length is stored as a single byte
            memcpy(key, &buffer[2], 4);
            len = lenIndic;
           
            encoded = (uint8_t*)malloc(len*sizeof(uint8_t));
            memcpy(encoded, &buffer[6], len);
        }
        else if(lenIndic == 126) { // Check if length is stored as two bytes
            memcpy(key, &buffer[4], 4);
            len = (unsigned char)buffer[2] << 8 | (unsigned char)buffer[3];
           
            //make sure len is not larger than msg buffer
            if(len < msgSize) { 
                encoded = (uint8_t*)malloc(len*sizeof(uint8_t));
                memcpy(encoded, &buffer[8], len);
            }
        }
        else if(lenIndic == 127) { // Check if length is stored as 3 bytes
            std::cout << "Recieved Message too large\n";
            return -1;
        }
        else {
            std::cout << "Unable to decode message\n";
            return -1;
        }

        for(int i = 0; i < len; i++) {
            //decode message using mask bits
            msg[i] = (encoded[i] ^ key[i & 0x3]);
        }
        free(encoded);
        //Moved to inside the if statment to prevent large unchecked values from being returned
        return len; 
    }
    else {
        std::cout << "No 129\n";
    }

    return -2;
}


//create frame before sending to client
int Web::WebsocketServer::create_frame(uint8_t buf[], char msg[], int len) {
    //add check for packet size later

    if(len < 126) {
        //use strncpy to create frame. May be less variabality than memset?
        buf[0] = 129;
        buf[1] = (unsigned char)len; //set mask bit to 0, and indicate message length

        //make sure len includes just the message and not any \r\n that may come after
        memcpy(&buf[2], msg, len); //copy message to buffer
        std::cout << " Short Frame created\n";

        return 0;
    }
    else if(len < 320000) { //something is going on with the length. When sending the proper length of the arr, it comes up one byte short
        uint16_t binLen = (uint16_t)len; //convert to network byte order. Even though it already should be
        std::cout << "From: " << len << " to: " << (int)binLen << "\n";

        //convert binLen into two 8 bit bytes so it can be read by websocket
        buf[0] = 129;
        buf[1] = (uint8_t)126 & 0b01111111; //make sure mask bit is set to 0
        buf[2] = (binLen >> 8) & 0xFF; //shift byte over to get the 8 MSB
        buf[3] = binLen & 0xFF; // get 8 LSB
        std::cout << "Bit1: " << (int)buf[2] << "\n";
        std::cout << "Bit2: " << (int)buf[3] << "\n";
        memcpy(&buf[4], msg, len);
        std::cout << "Long Frame created\n";
        return 0;
    }

    return -1;
}


//send packet to client
int Web::WebsocketServer::send_data(char msg[], int len) {
    //uint8_t packet[this->maxPktSize];
    uint8_t packet[this->maxPktSize];
    
    if(create_frame(packet, msg, len) == 0) {
        std::cout << "Sending data\n";

        if(packet[1] == (unsigned char)126) {
            uint16_t decLen = (packet[2] << 8) | packet[3]; 
            std::cout << "decoded LEN: " << (int)decLen << "\n";
            send(this->clientSocket, packet, len+4, 0); //length of packet must be correct, otherwise it will not be detected by client
            return 0;
        }
        send(this->clientSocket, packet, len+2, 0);
        return 0;
    }
    //print_frame(packet, len+6);
    return -1;
     
}


//break processess off in threads
void Web::WebsocketServer::threaded_listener() {
    std::cout << "threaded listener call\n";

    //create a listener thread and action thread

    //lambda function
    //use this as the capture clause so thread can access class variables
    //handle websocket connection
    this->pool.enqueue([this] {
        std::cout << "Listening for messages...\n";
        char *buffer = (char*)malloc(this->maxPktSize*sizeof(char));
        uint8_t msg[this->maxPktSize];
        memset(msg, '\0', this->maxPktSize);
        memset(buffer, '\0', this->maxPktSize);
        int g;
        while((g = recv(this->clientSocket, buffer, this->maxPktSize, 0)) > 0) {
            std::cout << "message recieved\n";
            if(buffer[0] != '\0') {
                int size = recv_data(buffer, this->maxPktSize, msg, this->maxPktSize);
                if(size > 0) {
                    if(size == 1) {
                        this->msg = msg[0];
                        this->modified = true;
                    }
                    
                    //reset buffers
                    memset(msg, '\0', this->maxPktSize);
                    memset(buffer, '\0', this->maxPktSize);
                }
            }
        }
        std::cout << "Error: " << g << "\n";
        std::cout << "Socket closed\n";
        free(buffer);
    });

    //repurpose this thread for sending messages to client
    this->pool.enqueue([this] {
        while(true) {
            //this->webAction = this->msg;
            if(msgQueue.size() > 0) {

            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}


//listen for incoming messages
void Web::WebsocketServer::listener() {
    std::cout << "Listening for messages...\n";
    char *buffer = (char*)malloc(this->maxPktSize*sizeof(char));
    uint8_t msg[this->maxPktSize];
    memset(msg, '\0', this->maxPktSize);
    memset(buffer, '\0', this->maxPktSize);
    int g;
    while((g = recv(this->clientSocket, buffer, this->maxPktSize, 0)) > 0) {
        std::cout << "message recieved\n";
        //make sure message is not empty
        if(buffer[0] != '\0') {
            int size = recv_data(buffer, this->maxPktSize, msg, this->maxPktSize);
            if(size > 0) {

                //reset buffers
                memset(msg, '\0', this->maxPktSize);
                memset(buffer, '\0', this->maxPktSize);
            }
        }
    }
    std::cout << "Error: " << g << "\n";
    std::cout << "Socket closed\n";
    free(buffer);
}


void Web::WebsocketServer::set_threading(bool opt) {
    this->thread = opt;
}


bool Web::WebsocketServer::get_threading() {
    return this->thread;
}

uint8_t Web::WebsocketServer::get_command() {
    if(this->modified == true) {
        this->modified = false;
        return this->msg;
    }
    return 0;
}
