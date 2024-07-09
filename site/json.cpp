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


void JsonObject::throw_error(bool a, std::string &expected) {
    if(a) {
        std::cout << "Error: Expected ',', got: " << expected << "\n";
    }
    else {
        std::cout << "Error: Expected ':', got: " << expected << "\n";
    }
}


//handle getting key, will always be string in my current use case
void JsonObject::parse() {
    std::string key;
    struct Token token = tokenizer.get_token(); //key indicate if moving onto next key
    //tokenizer.get_token(); //skip seperator like colon or comma
    
    while(this->validToken) {
        //tokenizer.get_token(); //skip over initial curley bracket
        if(tokenizer.has_tokens()) {
            std::cout << "Token is valid\n";
            token = tokenizer.get_token(); //skip seperator like colon or comma

            switch(token.type) {
                case(TOKEN::CURLEY_OPEN): {
                    //parse_object();
                    break;
                }
                case (TOKEN::STRING): {
                    if(this->setKey == true) {
                        //token = tokenizer.get_token();
                        key = token.c;
                        this->setKey = false;
                        //tokenizer.get_token(); //skip over :
                        std::cout << "Setting key " << key << "\n";
                    }
                    else {
                        (*this->list)[key] = parse_string(token);
                        this->setKey = true;
                    }
                    std::cout << "recieved string " << token.c << "\n";
                    break;
                }
                case (TOKEN::NUMBER): {
                    (*this->list)[key] = parse_number(token);
                    std::cout << "NUBMER RECIEVED: " << (*this->list)[key]->get_float() << "\n";
                    this->setKey = true;
                    break;
                }
                case (TOKEN::BOOLEAN): {
                   (*this->list)[key] = parse_boolean(token); 
                    this->setKey = true;
                    std::cout << "bool test: " << (*this->list)[key]->get_string() << "\n";
                    break;
                }
                case (TOKEN::NULL_TYPE): {
                    break;
                }
                default: {
                    if(token.type == TOKEN::CURLEY_CLOSE) {
                        this->validToken = false;
                    }
                    else if(this->setKey && token.type == TOKEN::COMMA) {
                        this->validToken = true;
                    }
                    else if(!this->setKey && token.type == TOKEN::COLON) {
                        this->validToken = true;
                    }
                    else {
                        throw_error(this->setKey, token.c);
                        this->validToken = false;
                    }
                }
            }
/*
            token = tokenizer.get_token();

            //check for valid next characters
            if(token.type == TOKEN::CURLEY_CLOSE) {
                break;
            }

            if(!this->setKey) {
                if(token.type != TOKEN::COLON) {
                }
            }
            else if(this->setKey) {
                if(token.type != TOKEN::COMMA) {
                }
            }
            else {
                std::cout << "Looks good\n";
            }
            */
        }
    }
}


JSON::JSONNode* JsonObject::parse_number(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    std::cout << "NUMBER VAL: " << token.c << "\n";
    node->set_string(token.c);
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


JSON::JSONNode* JsonObject::parse_boolean(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    if(token.c[0] == 't') {
        node->set_string("true");
        node->set_bool(true);
    }
    else if(token.c[0] == 'f') {
        node->set_string("false");
        node->set_bool(false);
    }

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
