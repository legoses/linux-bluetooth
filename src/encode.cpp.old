#include <encode.h>
#include <iostream>

/*
static char constexpr Encode::encode_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
    '3', '4', '5', '6', '7', '8', '9', '+', '/'};
*/

int Encode::base64_length(int len) {
    if(len%3 != 0) {
        if((len-1)%3 == 0) {
            return len + ((len-1)/3)+1;
        }
        else if((len-2)%3 == 0) {
            return len + ((len-2)/3)+2;
        }
    }

    return len + len/3;
}


void Encode::gen_sha_hash(const uint8_t input[], int inputSize, uint8_t *hashBuf)  {
    unsigned char digest[SHA_DIGEST_LENGTH] = {0};
    unsigned char *sha = SHA1(input, inputSize, hashBuf);

}


void Encode::gen_base64(uint8_t *digest, int digestSize, uint8_t *base64) {
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
        std::cout << "I: " << i << "\n";
    }

    //convert the remaining 2 bits
    std::cout << "I: " << i << "\n";
    std::cout << "math: " << digestSize - (i-2) << "\n";
    if((i-2) - digestSize == -2) {
        i-=3;
        base64-=4;
        uint32_t bits = (digest[i] << 16) | 0x00 << 8 | 0x00;
        std::cout << "base64i: " << base64i << "\n";
        
        base64[base64i] = encode_table[(bits >> 18) & 0b0011'1111];
        base64[base64i+1] = encode_table[(bits >> 12) & 0b0011'1111];
        base64[base64i+2] = '=';
        base64[base64i+3] = '=';
    }
    else if((i-2) - digestSize == -1){
        i-=3;
        base64-=4;
        uint32_t bits = (digest[i] << 16) | digest[i+1] << 8 | 0x00;
        
        std::cout << "base64i: lower " << base64i+3 << "\n";

        base64[base64i] = encode_table[(bits >> 18) & 0b0011'1111];
        base64[base64i+1] = encode_table[(bits >> 12) & 0b0011'1111];
        base64[base64i+2] = encode_table[(bits >> 6) & 0b0011'1111];
        base64[base64i+3] = '=';
        std::cout << digestSize-(i-2) << "\n";
        std::cout << "Fix this\n";
    }
}
