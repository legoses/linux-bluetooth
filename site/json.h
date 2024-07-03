#ifndef JSON_H
#define JSON_H

#include <stdint.h>
#include <iostream>
#include <map>
#include "jsonnode.h"

class JsonObject {
    uint8_t *initArr;
    int initArrSize;

    //keep track of data type
public:
    JsonObject(uint8_t array[], int arrSize);
    void printArr();
};


#endif
