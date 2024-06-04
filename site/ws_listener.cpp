#include "ws_listener.h"
#include "threadpool.h"

WSListener::WSListener(void (*funcptr)(uint8_t[], int)) {
    this->func_cb = funcptr;
}

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


void WSListener::threaded_listener() {
    /*
    std::cout << "Threaded listening for messages...\n";

    //create a thread to listen for events on the websocket, and another to execute recieved events
    char *buffer = (char*)malloc(this->maxPktSize*sizeof(char));
    //uint8_t msg[this->maxPktSize];
    //will probable need to have this called in a thread so messages to not overwrite eachother
    uint8_t *msg = (uint8_t*)malloc(this->maxPktSize*sizeof(uint8_t));
    memset(msg, '\0', this->maxPktSize);
    memset(buffer, '\0', this->maxPktSize);
    int g;

    while((g = recv(this->clientSocket, buffer, this->maxPktSize, 0)) > 0) {
        std::cout << "message recieved\n";
        if(buffer[0] != '\0') {
            int size = recv_data(buffer, this->maxPktSize, msg, this->maxPktSize);
            std::cout << "Size test: " << size << "\n";
            if(size > 0) {
                //instead of constantly creating and destroying threads, create thread pool
                //std::thread threaded_cb(this->func_cb, std::ref(msg), size);
                holdThread.push_back(std::thread(this->func_cb, std::ref(msg), size));

                //Test to JS console has something to output
                //delete later
                char testMsg[] = "tstMsg";
                send_data(testMsg, sizeof(testMsg)-1);

                
                //reset buffers
                memset(msg, '\0', size);
                memset(buffer, '\0', g);
            }
        }
    }
    std::cout << "Error: " << g << "\n";
    std::cout << "Socket closed\n";
    free(buffer);
    free(msg);
    */

    ThreadPool pool(2);

    //lambda function
    //use this as the capture clause so thread can access class variables
    pool.enqueue(listener);

    return 0;
}
