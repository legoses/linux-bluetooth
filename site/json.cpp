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

void JsonObject::parse() {
    while(tokenizer.has_tokens()) {
        //should be curley
        this->root = tokenizer.get_token(); //key
        this->current = tokenizer.get_token(); //value


        switch(this->current.type) {
            case(TOKEN::CURLEY_OPEN): {
                //parse_object();
                break;
            }
            case (TOKEN::STRING): {
                //this->list[root.c] = parse_string();
                std::cout << "Printing root val\n";
                std::cout << this->root.c << "\n";
                break;
            }
            case (TOKEN::NUMBER): {

            }
            case (TOKEN::COLON): {

            }
            case (TOKEN::COMMA): {

            }
            case (TOKEN::NULL_TYPE): {

            }
            case (TOKEN::CURLEY_CLOSE): {

            }
            case (TOKEN::ARRAY_OPEN): {

            }
            case (TOKEN::ARRAY_CLOSE): {

            }
            //free(this->root.c);
            //free(this->root);
        }
    }
}


JSON::JSONNode* JsonObject::parse_string() {
    JSON::JSONNode *node = new JSON::JSONNode();
    node->set_string(this->current.c);
    //node->charLen = this->current.charLen;
    //free(current.c);
    //free(current);
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
