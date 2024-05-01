#include <iostream>
#include "encodeTest.cpp"

int main() {
    uint8_t hashtxt[] = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    //uint8_t test[] = "this is a 20 charact";
    int testSize = sizeof(hashtxt)-1;
    
    int total = base64_length(testSize);
    uint8_t base[total];
    uint8_t *digest = (uint8_t*)malloc(SHA_DIGEST_LENGTH*sizeof(uint8_t));


    gen_sha_hash(hashtxt, testSize, digest);
    gen_base64(hashtxt, SHA_DIGEST_LENGTH, base, total);
    std::cout << "total: " << total << "\n";
    for(int i = 0; i < total; i++) {
        std::cout << base[i];
    }
    std::cout << "\n";
    free(digest);
    return 0;
}
