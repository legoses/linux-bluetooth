#include <iostream>
#include <openssl/sha.h>
#include <stdint.h>


char constexpr encode_table[] {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
    '3', '4', '5', '6', '7', '8', '9', '+', '/'};


int base64_length(int len) {
    int total = len/3*4;
    
    if(len%3 != 0) {
        total+=4;
    }
    return total;
}


void gen_sha_hash(const uint8_t input[], int inputSize, uint8_t *hashBuf)  {
    unsigned char digest[SHA_DIGEST_LENGTH] = {0};
    //unsigned char *sha = SHA1(input, inputSize, digest);
    unsigned char *sha = SHA1(input, inputSize, hashBuf);

}


void gen_base64(uint8_t *digest, int digestSize, uint8_t *base64, int baseSize) {
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
    std::cout << "base size: " << baseSize << "\n";
    std::cout << "base64i: " << base64i << "\n";
    if(digestSize%3 == 2) {
        i-=3;
        base64-=4;
        uint32_t bits = (digest[i] << 16) | 0x00 << 8 | 0x00;
        std::cout << "base64i: " << base64i << "\n";
       
        if(base64i+3 < baseSize) {
            //base64[base64i] = encode_table[(bits >> 18) & 0b0011'1111];
            //base64[base64i+1] = encode_table[(bits >> 12) & 0b0011'1111];
            //base64[base64i+2] = '=';
            //base64[base64i+3] = '=';
            
            base64[baseSize-3] = encode_table[(bits >> 18) & 0b0011'1111];
            base64[baseSize-2] = encode_table[(bits >> 12) & 0b0011'1111];
            base64[baseSize-1] = '=';
            base64[baseSize] = '=';
        }
    }
    //else if((i-2) - digestSize == -1){
    else if(digestSize%3 == 2) {
        i-=3;
        base64-=4;
        uint32_t bits = (digest[i] << 16) | digest[i+1] << 8 | 0x00;
        
        std::cout << "base64i: lower " << base64i+3 << "\n";

        if(base64i+3 < baseSize) {
            //base64[base64i] = encode_table[(bits >> 18) & 0b0011'1111];
            //base64[base64i+1] = encode_table[(bits >> 12) & 0b0011'1111];
            //base64[base64i+2] = encode_table[(bits >> 6) & 0b0011'1111];
            //base64[base64i+3] = '=';
            base64[baseSize-3] = encode_table[(bits >> 18) & 0b0011'1111];
            base64[baseSize-2] = encode_table[(bits >> 12) & 0b0011'1111];
            base64[baseSize-1] = '=';
            base64[baseSize] = '=';
        }
    }
    std::cout << "Digest size: " << digestSize << "\n";
    std::cout << "i: " << i+3 << "\n";
    std::cout << "base64i: " << base64i+3 << "\n";
}

/*
int main() {
    //int hashSize = SHA_DIGEST_LENGTH*2+1;
    //sha1 hash is 20 btyes
    int hashSize = 20;
    uint8_t *hashBuf = (uint8_t*)malloc(hashSize*sizeof(uint8_t));
    uint8_t testStr[] = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    //uint8_t base64[60];
    uint8_t *base64 = (uint8_t*)malloc(base64_length(hashSize)*sizeof(uint8_t));

    std::cout << "Length of base64: " << base64_length(hashSize) << "\n";

    gen_sha_hash(testStr, sizeof(testStr)-1, hashBuf);
    std::cout << "Hash generated: " << hashBuf << "\n";

    //gen_base64(hashBuf, hashSize, base64);
    gen_base64(hashBuf, hashSize, base64);
    std::cout << "Base64 generated: ";
    for(int i = 0; i < 28; i++) {
        std::cout << base64[i];
    }
    free(hashBuf);
    std::cout << "\n";
    return 0;
}
*/
