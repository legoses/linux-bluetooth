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
    struct Token root;
    struct Token current;
    //bool root = true; //indicate whether workign with key or value

    //main object to store parsed json
    JSON::JSONObject *list = new JSON::JSONObject();

    void parse();
    JSON::JSONNode* parse_object();
    JSON::JSONNode* parse_list();
    JSON::JSONNode* parse_string();
    JSON::JSONNode* parse_number();
    JSON::JSONNode* parse_boolean();
    JSON::JSONNode* parse_null();
    
    

    //keep track of data type
public:
    JsonObject(uint8_t array[], int arrSize);
    void printArr();
};


#endif
