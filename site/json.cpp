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
        struct Token token = tokenizer.get_token();

        switch(token.type) {
            case(TOKEN::CURLEY_OPEN): {

            }
            case (TOKEN::STRING): {

            }
            case (TOKEN::NUMBER): {

            }
            case (TOKEN::COLON): {

            }
            case (TOKEN::COMMA): {

            }
            case (TOKEN::NULL_TYPE): {

            }
            case (TOKEN::CURLEY_OPEN): {

            }
            case (TOKEN::CURLEY_CLOSE): {

            }
            case (TOKEN::ARRAY_OPEN): {

            }
            case (TOKEN::ARRAY_CLOSE): {

            }
        }
    }
}
