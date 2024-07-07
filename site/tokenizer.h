#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdint.h>
#include <string>

enum class TOKEN {
    CURLEY_OPEN,
    CURLEY_CLOSE,
    COLON,
    STRING,
    NUMBER,
    ARRAY_OPEN,
    ARRAY_CLOSE,
    COMMA,
    BOOLEAN,
    NULL_TYPE
};

struct Token {
    std::string c;
    TOKEN type;
};

class Tokenizer {
    int arrSize;
    int curPos = 0;
    uint8_t *char_array;

    //char char_array[];

public:
    //when get token is called, will loop through file for length of single token
    //keep track of position so it can continue if called again
    Tokenizer(uint8_t *arr, int size);

    bool has_tokens();

    struct Token get_token();


};

#endif
