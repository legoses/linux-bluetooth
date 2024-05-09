#include "websocket.h"
//#include <future>

/*
 * TODO:
 * Websocket will have callback on data recieve. This will take data and act on bluetooth portion of program.
 * This will likely be some sort of async or threaded processes
 *
 * Seperate class will handle socket, and probably be passed to websocket server class. Or maybe class can handle multiple connections on its own?
 * Create function pointer to pass to class. This can be called from inside the class
 */

void websocket_cb(uint8_t msg[], int size) {
    for(int i = 0; i < size; i++) {
        std::cout << msg[i];
    }
    std::cout << "\n";
}


int main() {
    //set up async listener outsize of class. Listener will call function pointer as async function
    uint8_t serverListen[] = "0.0.0.0";
    Web::WebsocketServer server(8080);
    //void (*callback)(uint8_t[], int);
    //callback = websocket_cb;
    server.set_cb(websocket_cb);

    server.begin();
    
    return 0;
}

/*
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

    int headerSize;
    char *wsHeader = create_ws_header(readBuffer, bufSize, headerSize);

    if(wsHeader != NULL) {
        send(clientSocket, wsHeader, headerSize-1, 0);
        memset(buffer, '\0', bufSize);
        recv(clientSocket, buffer, bufSize, 0);
        //read(clientSocket, buffer)
       
        //uint8_t *connBuf = (uint8_t*)malloc(bufSize*sizeof(uint8_t));
        //detect if websocket connection was successful
        if(buffer[0] != 0) {
            std::cout << "Websocket Connection Successful\n";
            uint8_t msg[bufSize];
            memset(msg, '\0', bufSize);

            while(true) {
                int msgLen = recv_data(buffer, bufSize, msg, bufSize);
                if(msgLen != -1) {
                    std::cout << "Recieved message: " << msg << "\n";
                    memset(msg, '\0', msgLen);
                }
                recv(clientSocket, buffer, bufSize, 0);

                sleep(1);
            }
            //free(connBuf);
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
*/

