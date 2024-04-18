#ifndef ENCODE_H
#define ENCODE_H

#include <openssl/sha.h>
#include <stdint.h>

class Encode {
    char constexpr encode_table[] {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
        'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
        'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
        's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
        '3', '4', '5', '6', '7', '8', '9', '+', '/'};
    
    public:
    int base64_length(int len);

    void gen_sha_hash(const uint8_t input[], int inputSize, uint8_t *hashBuf);

    void gen_base64(uint8_t *digest, int digestSize, uint8_t *base64);
};

#endif
