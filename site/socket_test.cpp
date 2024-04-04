#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_LOOPBACK;

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    const char *message = "Test message";
    send(clientSocket, message, strlen(message), 0);

    close(clientSocket);
    return 0;
}

