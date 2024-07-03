#ifndef JSON_NODE_H
#define JSON_NODE_H

#include "using_vars.h"

class JSONNode {
    enum class Type {
        OBJECT,
        LIST,
        STRING,
        BOOLEAN,
        NULL_TYPE,
        NUMBER,
        STRING
    };

    //Creates an object that contains one of these values
    //only takes the space of the largest value
    union Values {
        JSONObject *object;
        JSONList *list;
        char *s;
        float fVal;
        bool bVal;
    } values;

    Type type;
};

#endif
