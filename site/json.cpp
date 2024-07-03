//#include <json.h>
#include "json.h"

JsonObject::JsonObject(uint8_t array[], int arrSize)
    : initArr{array}
    , initArrSize(arrSize) {

    for(int i = 0; i < arrSize; i++) {

    }
}

void JsonObject::printArr() {
    std::cout << this->initArr << "\n";
} 
