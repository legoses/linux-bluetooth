int Websockets::get_websocket_key(char *header, const int headerSize, unsigned char buffer[], int bufferSize) {
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


void Websockets::gen_base64(uint8_t *digest, int digestSize, uint8_t *base64) {
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


void Websockets::gen_sha_hash(const unsigned char input[], int inputSize, uint8_t *hashBuf)  {
    unsigned char digest[SHA_DIGEST_LENGTH] = {0};
    //unsigned char *sha = SHA1(input, inputSize, digest);
    unsigned char *sha = SHA1(input, inputSize, hashBuf);
    /*
    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        std::cout << "testone: " << digest << "\n";
        sprintf(&hashBuf[i*2], "%02x", (unsigned int)digest[i]);
        std::cout << "test two: " << hashBuf << "\n";
    }
    */

}


int upgrade_to_ws(char *readBuffer, const int bufSize, int conn) {
    std::cout << "Upgrading connection\n";
    uint8_t webkey[60];
    memset(webkey, '\0', sizeof(webkey));

    char initReg[] = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";

    if(get_websocket_key(readBuffer, bufSize, webkey, sizeof(webkey)) == 0) {
        std::cout << webkey << "\n";
        //allocate 2 more than needed forr \r\n return characters
        int baseSize = 60; //figure out a way to calculate this instead of hard coding
        uint8_t base64[baseSize];
        int hashSize = SHA_DIGEST_LENGTH*2+1;
        uint8_t *hashBuf = (uint8_t*)malloc(hashSize*sizeof(uint8_t));

        gen_sha_hash(webkey, sizeof(webkey)-1, hashBuf);

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
