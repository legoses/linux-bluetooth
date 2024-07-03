#ifndef TOKENIZER_H
#define TOKENIZER_H

class Tokenizer {
    int arrSize;
    char array[];
    int curPos = 0;

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
        char *c = nullptr;
        TOKEN type;
    };

public:
    //when get token is called, will loop through file for length of single token
    //keep track of position so it can continue if called again
    Tokenizer(char arr[], int size);

    bool has_tokens();

    struct Token get_token();


};

#endif
