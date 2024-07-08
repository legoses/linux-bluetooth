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
                //this->list[root.c] = parse_string();
                //std::cout << this->root.c << "\n";
                //std::cout << this->current.c << "\n\n\n";
                //this->root = tokenizer.get_token();
                JSON::JSONNode *node = parse_string(token);
                this->setKey = true;
                //tokenizer.get_token();
                
                break;
            }
            case (TOKEN::NUMBER): {

            }
            case (TOKEN::BOOLEAN): {

            }
            case (TOKEN::NULL_TYPE): {

            }
        }
    }
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
