//#include <json.h>
#include "json.h"

JsonObject::JsonObject(uint8_t array[], int arrSize)
    : initArr{array}
    , initArrSize(arrSize)
    , tokenizer(array, arrSize) {
   parse(); 
}

/*
JsonObject::~JsonObject() {

}
*/

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
            token = tokenizer.get_token(); //skip seperator like colon or comma

            switch(token.type) {
                case(TOKEN::CURLEY_OPEN): {
                    (*this->list)[key] = parse_object();
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
                    //(*this->list)[key]->set_string(token.c);
                    std::cout << "NUBMER RECIEVED: " << (*this->list)[key]->get_float() << "\n";
                    this->setKey = true;
                    break;
                }
                case (TOKEN::BOOLEAN): {
                    (*this->list)[key] = parse_boolean(token); 
                    //(*this->list)[key]->set_string(token.c);
                    this->setKey = true;
                    break;
                }
                case (TOKEN::NULL_TYPE): {
                    JSON::JSONNode *node = new JSON::JSONNode();
                    node->set_string("null");
                    break;
                }
                case (TOKEN::ARRAY_OPEN): {
                    (*this->list)[key] = parse_array();
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
        }
    }
}


JSON::JSONNode* JsonObject::parse_number(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    node->set_string(token.c);
    node->set_float(std::stof(token.c));
    node->set_type(TOKEN::NUMBER);
    return node;
}


JSON::JSONNode* JsonObject::parse_string(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    node->set_string(token.c);
    node->set_type(TOKEN::STRING);

    //string must be deleted on deconstruction
    return node; 
}


JSON::JSONNode* JsonObject::parse_boolean(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    if(token.c[0] == 't') {
        node->set_string(token.c);
        node->set_bool(true);
    }
    else if(token.c[0] == 'f') {
        node->set_string("false");
        node->set_bool(false);
    }
    node->set_type(TOKEN::BOOLEAN);

    return node;
}


JSON::JSONNode* JsonObject::parse_object() {
    JSON::JSONNode *node = new JSON::JSONNode();
    JSON::JSONObject *obj = new JSON::JSONObject();
    std::string key;
    struct Token token = tokenizer.get_token(); //key indicate if moving onto next key
    //tokenizer.get_token(); //skip seperator like colon or comma
    
    while(this->validToken) {
        //tokenizer.get_token(); //skip over initial curley bracket
        if(tokenizer.has_tokens()) {
            token = tokenizer.get_token(); //skip seperator like colon or comma

            switch(token.type) {
                case(TOKEN::CURLEY_OPEN): {
                    (*obj)[key] = parse_object();
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
                        (*obj)[key] = parse_string(token);
                        this->setKey = true;
                    }
                    std::cout << "recieved string " << token.c << "\n";
                    break;
                }
                case (TOKEN::NUMBER): {
                    (*obj)[key] = parse_number(token);
                    //(*obj)[key]->set_string(token.c);
                    std::cout << "NUBMER RECIEVED: " << (*obj)[key]->get_float() << "\n";
                    this->setKey = true;
                    break;
                }
                case (TOKEN::BOOLEAN): {
                    (*obj)[key] = parse_boolean(token); 
                    //(*obj)[key]->set_string(token.c);
                    this->setKey = true;
                    break;
                }
                case (TOKEN::NULL_TYPE): {
                    JSON::JSONNode *node = new JSON::JSONNode();
                    node->set_string("null");
                    break;
                }
                case (TOKEN::ARRAY_OPEN): {
                    (*obj)[key] = parse_array();
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
        }
    }
    node->set_type(TOKEN::OBJECT);
    node->set_object(obj);
    return node;
}

//make sure array closes before a stray } is detected, and last value does not have a ,
JSON::JSONNode* JsonObject::parse_array() {
    JSONNode *node = new JSONNode(); //will be parent node to store list
    JSONList lst* = new JSONList();

    bool cont = true;
    struct Token token = tokenizer.get_token();
    
    while(true) {
        switch(token.type) {
            case :
        }
    }
    
}


JSON::JSONNode* JsonObject::get_item(const std::string &key) {
    return (*this->list)[key];
}

