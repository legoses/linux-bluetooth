#include "tokenizer.h"

Tokenizer::Tokenizer(char arr[], int size)
    : array{arr}
    , arrSize(size) {}

bool Tokenizer::has_tokens() {
    if(curPos < arrSize) {
        return true;
    }
    return false;
}


//detect token type and create struct object
struct Token Tokenizer::get_token() {
    struct Token token;

    if(this->array[curPos] == '{') {
        token.type = TOKEN::CURLEY_OPEN;
        curPos++;
    }
    else if(this->array[curPos] == '}') {
        token.type = TOKEN::CURLEY_CLOSE;
        curPos++;
    }
    else if(this->array[curPos] == '"') {
        int initPos = curPos;
        int count = 1;
        
        token.type = TOKEN::STRING;
        curPos++;

        while(this->array[curPos] != '"' && curPos < arrLen) {
            count++;
            curPos++;
        }
        token.c = (char*)malloc(count*sizeof(str));
        memcpy(token.c, &this->array[initPos], count);
    }
    else if(this->array[curPos] == ':') {
        token.type = TOKEN::COLON;
        curPos++;
    }
    else if(this->array[curPos] == '-' || (this->array[curPos] >= 0 && this->array[curPos] <= 9)) {
        int initPos = curPos;
        int count = 1;
        while((this->array[curPos] == '-' || (this->array[curPos] >= 0 && this->array[curPos] <= 9)) && curPos < arrLen) {
            curPos++;
            count++;
        }
        token.c = (char*)malloc(count*sizeof(str));
        memcpy(token.c, &this->array[initPos], count);
    }
    else if(this->array[curPos] == '[') {
        token.type = TOKEN::ARRAY_OPEN;
        curPos++;
    }
    else if(this->array[curPos] == ']') {
        token.type = TOKEN::ARRAY_CLOSE;
        curPos++;
    }
    else if(this->array[curPos] == 'f') {
        token.type = TOKEN::BOOLEAN;
        token.c = (char*)malloc(5*sizeof(char));
        memcpy(token.c, "false", 5*sizeof(char));
        curPos += 5;
    }
    else if(this->array[curPos] == 't') {
        token.type = TOKEN::BOOLEAN;
        token.c = (char*)malloc(4*sizeof(char));
        memcpy(token.c, "true", 4*sizeof(char));
        curPos += 4;
    }
    else if(this->array[curPos] == ',') {
        token.type = TOKEN::COMMA;
        curPos++;
    }
    else if(this->array[curPos] == 'n') {
        token.type = TOKEN::NULL_TYPE;
        token.c = (char*)malloc(3*sizeof(char));
        memcpy(token.c, "null", 3*sizeof(char));
        curPos += 3;
    }

    return token;
}
