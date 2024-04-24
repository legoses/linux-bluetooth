#include <iostream>
#include "encodeTest.cpp"

int main() {
    uint8_t test[] = "this is a 20 charact";
    int testSize = sizeof(test)-1;
    std::cout << "str size: " << testSize << "\n";
    int total = base64_length(testSize);
    uint8_t base[total];
    gen_base64(test, 20, base, total);
    std::cout << "total: " << total << "\n";
    for(int i = 0; i < total; i++) {
        std::cout << base[i];
    }
    std::cout << "\n";
    return 0;
}
