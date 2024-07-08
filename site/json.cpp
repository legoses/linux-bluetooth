//#include <json.h>
#include "json.h"

JsonObject::JsonObject(uint8_t array[], int arrSize)
    : initArr{array}
    , initArrSize(arrSize)
    , tokenizer(array, arrSize) {
   parse(); 
}

void JsonObject::printArr() {
    std::cout << this->initArr << "\n";
} 


//handle getting key, will always be string in my current use case
void JsonObject::parse() {
    std::string key;
    //tokenizer.get_token(); //skip seperator like colon or comma
    
    while(tokenizer.has_tokens()) {
        //tokenizer.get_token(); //skip over initial curley bracket
        struct Token token = tokenizer.get_token(); //key
        
        //indicate if moving onto next key
        if(this->setKey) {
            token = tokenizer.get_token();
            key = token.c;
            std::cout << "Key is " << key << "\n";
            this->setKey = false;
            tokenizer.get_token(); //skip colon
            token = tokenizer.get_token();
        }

        //token = tokenizer.get_token(); //check for value

        switch(token.type) {
            case(TOKEN::CURLEY_OPEN): {
                //parse_object();
                break;
            }
            case (TOKEN::STRING): {
                (*this->list)[key] = parse_string(token);
                this->setKey = true;
                break;
            }
            case (TOKEN::NUMBER): {
                (*this->list)[key] = parse_number(token); //oh man i dont know why this crashes
                std::cout << "NUBMER RECIEVED: " << (*this->list)[key]->get_float() << "\n";
                this->setKey = true;
                break;
            }
            case (TOKEN::BOOLEAN): {

            }
            case (TOKEN::NULL_TYPE): {

            }
        }
    }
}


JSON::JSONNode* JsonObject::parse_number(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    std::cout << "NUMBER VAL: " << token.c << "\n";
    node->set_float(std::stof(token.c));
    return node;
}


JSON::JSONNode* JsonObject::parse_string(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    node->set_string(token.c);

    //string must be deleted on deconstruction
    std::cout << "value: " << *node->get_string() << "\n";
    return node; 
}

/*
void JsonObject::parse_object() {
    JSON::JSONNode *obj = new JSON::JSONNode();

    while(tokenizer.has_tokens()) {
        //should be curley

        switch(token.type) {
            case(TOKEN::CURLEY_OPEN): {
                parse_object();
                break;
            }
            case (TOKEN::STRING): {
                parse_string();
                break;
            }
            case (TOKEN::NUMBER): {
                parse_number();
                break;
            }
            case (TOKEN::BOOLEAN): {

            }
            case (TOKEN::CURLEY_CLOSE): {

            }
            case (TOKEN::ARRAY_OPEN): {

            }
        }
    }
}
*/
