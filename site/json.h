#ifndef JSON_H
#define JSON_H

#include <stdint.h>
#include <iostream>
#include <map>
#include "jsonnode.h"
#include "tokenizer.h"

class JsonObject {
    uint8_t *initArr;
    int initArrSize;
    Tokenizer tokenizer;

    //main object to store parsed json
    JSONObject *list = new JSONObject();

    void parse();

    //keep track of data type
public:
    JsonObject(uint8_t array[], int arrSize);
    void printArr();
};


#endif
