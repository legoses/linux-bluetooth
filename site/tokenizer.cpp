#include "tokenizer.h"

Tokenizer::Tokenizer(uint8_t *arr, int size)
    : char_array{arr}
    , arrSize(size) {}

bool Tokenizer::has_tokens() {
    if(this->curPos >= this->arrSize) {
        return false;
    }
    else if(this->char_array[this->curPos + 1] == '}') {
        return false;
    }

    return true;
}


//detect token type and create struct object
struct Token Tokenizer::get_token() {
    struct Token token;

    if(this->char_array[curPos] == '{') {
        token.type = TOKEN::CURLEY_OPEN;
        curPos++;
    }
    else if(this->char_array[curPos] == '}') {
        token.type = TOKEN::CURLEY_CLOSE;
        curPos++;
    }
    else if(this->char_array[curPos] == '"') {
        int initPos = curPos;
        int count = 1;
        
        token.type = TOKEN::STRING;
        curPos++;

        while(this->char_array[curPos] != '"' && curPos < this->arrSize) {
            //count++;
            token.c += this->char_array[curPos];
            curPos++;
        }
        //token.c = (char*)malloc(count*sizeof(str));
        //memcpy(token.c, &this->char_array[initPos], count);
        //token.charLen = count;
    }
    else if(this->char_array[curPos] == ':') {
        token.type = TOKEN::COLON;
        curPos++;
    }
    else if(this->char_array[curPos] == '-' || (this->char_array[curPos] >= 0 && this->char_array[curPos] <= 9)) {
        int initPos = curPos;
        int count = 1;
        while((this->char_array[curPos] == '-' || (this->char_array[curPos] >= 0 && this->char_array[curPos] <= 9)) && curPos < this->arrSize) {
            token.c += this->char_array[curPos];
            curPos++;
        }
        //token.c = (char*)malloc(count*sizeof(str));
        //memcpy(token.c, &this->char_array[initPos], count);
    }
    else if(this->char_array[curPos] == '[') {
        token.type = TOKEN::ARRAY_OPEN;
        curPos++;
    }
    else if(this->char_array[curPos] == ']') {
        token.type = TOKEN::ARRAY_CLOSE;
        curPos++;
    }
    else if(this->char_array[curPos] == 'f') {
        token.type = TOKEN::BOOLEAN;
        token.c = "false";
        //token.c = (char*)malloc(5*sizeof(char));
        //memcpy(token.c, "false", 5*sizeof(char));
        curPos += 5;
    }
    else if(this->char_array[curPos] == 't') {
        token.type = TOKEN::BOOLEAN;
        token.c = "true";
        //token.c = (char*)malloc(4*sizeof(char));
        //memcpy(token.c, "true", 4*sizeof(char));
        curPos += 4;
    }
    else if(this->char_array[curPos] == ',') {
        token.type = TOKEN::COMMA;
        curPos++;
    }
    else if(this->char_array[curPos] == 'n') {
        token.type = TOKEN::NULL_TYPE;
        token.c = "null";
        //token.c = (char*)malloc(3*sizeof(char));
        //memcpy(token.c, "null", 3*sizeof(char));
        curPos += 3;
    }

    return token;
}