#ifndef WEB_SOCKET_H
#define WEB_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <string>
#include <encode.h>

class Websockets : public Encode {
    public:
    int get_websocket_key(char *header, const int headerSize, unsigned char buffer[], int bufferSize);

    void gen_base64(uint8_t *digest, int digestSize, uint8_t *base64);

    void gen_sha_hash(const unsigned char input[], int inputSize, uint8_t *hashBuf);

    int upgrade_to_ws(char *readBuffer, const int bufSize, int conn);
};

#endif
