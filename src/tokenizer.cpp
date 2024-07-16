#include <tokenizer.h>

Tokenizer::Tokenizer(uint8_t *arr, int size)
    : char_array{arr}
    , arrSize(size) {}

bool Tokenizer::has_tokens() {
    if(this->curPos >= this->arrSize) {
        return false;
    }
    //else if(this->char_array[this->curPos + 1] == '}') {
    //    return false;
    //}

    return true;
}


//detect token type and create struct object
struct Token Tokenizer::get_token() {
    struct Token token;
    //int testNum = this->char_array[curPos] - 48;
    uint8_t uLow = '0';
    uint8_t uHigh = '9';
    uint8_t uDash = '-';

    if(this->char_array[curPos] == '{') {
        token.type = TOKEN::CURLEY_OPEN;
        token.c = '{';
        curPos++;
    }
    else if(this->char_array[curPos] == '}') {
        token.type = TOKEN::CURLEY_CLOSE;
        token.c = '}';
        curPos++;
    }
    else if(this->char_array[curPos] == '"') {
        int initPos = curPos;
        int count = 1;
        
        token.type = TOKEN::STRING;
        curPos++;

        while(this->char_array[curPos] != '"' && curPos < this->arrSize) {
            token.c += this->char_array[curPos];
            curPos++;
        }
        curPos++;
    }
    else if(this->char_array[curPos] == ':') {
        token.type = TOKEN::COLON;
        token.c = ':';
        curPos++;
    }
    else if(this->char_array[curPos] == uDash || (this->char_array[curPos] >= uLow && this->char_array[curPos] <= uHigh)) { //value is number
        while(this->char_array[curPos] == uDash || (this->char_array[curPos] >= uLow && this->char_array[curPos] <= uHigh)) {
            token.c += this->char_array[curPos]; //convert to compatible format for string
            curPos++;
        }
        token.type = TOKEN::NUMBER;
    }
    else if(this->char_array[curPos] == '[') {
        token.type = TOKEN::ARRAY_OPEN;
        token.c = '[';
        curPos++;
    }
    else if(this->char_array[curPos] == ']') {
        token.type = TOKEN::ARRAY_CLOSE;
        token.c = ']';
        curPos++;
    }
    else if(this->char_array[curPos] == 'f') {
        token.type = TOKEN::BOOLEAN;
        token.c = "false";
        curPos += 5;
    }
    else if(this->char_array[curPos] == 't') {
        token.type = TOKEN::BOOLEAN;
        token.c = "true";
        curPos += 4;
    }
    else if(this->char_array[curPos] == ',') {
        token.type = TOKEN::COMMA;
        token.c = ',';
        curPos++;
    }
    else if(this->char_array[curPos] == 'n') {
        token.type = TOKEN::NULL_TYPE;
        token.c = "null";
        curPos += 3;
    }
    else {
        std::cout << "No valid option found for: " << this->char_array[curPos] << "\n";
    }

    return token;
}


struct Token Tokenizer::view_next_token() {
    int prevPos = this->curPos;
    struct Token token = get_token();
    this->curPos = prevPos;
    return token;
}
