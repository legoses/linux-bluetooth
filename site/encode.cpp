#include "encode.h"

int Encode::base64_length(int len) {
    int total = len/3*4;
    
    if(len%3 != 0) {
        total+=4;
    }
    return total;
}


int Encode::calc_utf_size(int strSize) {
    return (4*strSize)+1;
}


//void char_to_utf8(char *utfString, int utfSize, char *asciiString, int asciiStringSize) {
//   if(asciiString != NULL) {
//        if(utf8proc_utf8fromu(asciiString, asciiStringSize, utfString, utfSize) != 0) {
//            std::cout << "UTF-8 conversion failed\n";
//        }
//    } 
//}


void Encode::gen_sha_hash(uint8_t input[], int inputSize, uint8_t *hashBuf)  {
    unsigned char digest[SHA_DIGEST_LENGTH] = {0};
    //unsigned char *sha = SHA1(input, inputSize, digest);
    unsigned char *sha = SHA1(input, inputSize, hashBuf);
}


void Encode::gen_base64(uint8_t *digest, int digestSize, uint8_t *base64, int baseSize) {
    int base64i = 0;
    int i = 0;
    //printf("Printing hash: %x\n", digest);
    std::cout << "Printing hex: ";
    std::cout << digest;
    std::cout << "\n";
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
    i-=3;
    base64i-=4;

    //convert the remaining 2 bits
    if(digestSize%3 == 1) {
        uint32_t bits = (digest[i] << 16) | 0x00 << 8 | 0x00;
       
        if(base64i+3 <= baseSize) {
            std::cout << "rightious brother\n";
            base64[base64i] = encode_table[(bits >> 18) & 0b0011'1111];
            base64[base64i+1] = encode_table[(bits >> 12) & 0b0011'1111];
            base64[base64i+2] = '=';
            base64[base64i+3] = '=';
        }
    }
    else if(digestSize%3 == 2) {
        uint32_t bits = (digest[i] << 16) | digest[i+1] << 8 | 0x00;
        
        if(base64i+3 <= baseSize) {
            base64[base64i] = encode_table[(bits >> 18) & 0b0011'1111];
            base64[base64i+1] = encode_table[(bits >> 12) & 0b0011'1111];
            base64[base64i+2] = encode_table[(bits >> 6) & 0b0011'1111];
            base64[base64i+3] = '=';
        }
    }
    std::cout << "init base 64: " << base64 << "\n";
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
