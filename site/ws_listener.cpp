#include "ws_listener.h"

void WSListener::listener() {
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
            std::cout << "Size test: " << size << "\n";
            if(size > 0) {
                char tstMsg[] = "hello";
                this->func_cb(msg, size);
                send_data(tstMsg, sizeof(tstMsg)-1);
                
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
