#include <iostream>
#include "encodeTest.cpp"

int main() {
    char test[] = "this is a longer test test";
    int testSize = sizeof(test)-1;
    std::cout << "str size: " << testSize << "\n";
    int total = base64_length(testSize);
    std::cout << "total: " << total << "\n";
    return 0;
}
