#ifndef JSON_H
#define JSON_H

#include <stdint.h>
#include <iostream>
#include <map>
#include "jsonnode.h"
#include "tokenizer.h"
#include "enum_types.h"

class JsonObject {
    uint8_t *initArr;
    int initArrSize;
    Tokenizer tokenizer;
    bool setKey = true;
    bool validToken = true;
    //bool root = true; //indicate whether workign with key or value

    //main object to store parsed json
    JSON::JSONObject *list = new JSON::JSONObject();

    void parse();
    JSON::JSONNode* parse_object();
    JSON::JSONNode* parse_array();
    JSON::JSONNode* parse_string(struct Token &token);
    JSON::JSONNode* parse_number(struct Token &token);
    JSON::JSONNode* parse_boolean(struct Token &token);
    JSON::JSONNode* parse_null(struct Token &token);
    JSON::JSONNode* detect_type(struct Token &token);
    void throw_error(bool a, std::string &expected);
    
    

    //keep track of data type
public:
    JsonObject(uint8_t array[], int arrSize);
    void printArr();
    JSON::JSONNode* get_item(const std::string &key);
};


#endif
