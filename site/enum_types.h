#ifndef ENUM_TYPES_H
#define ENUM_TYPES_H


enum class Type {
        OBJECT,
        LIST,
        STRING,
        BOOLEAN,
        NULL_TYPE,
        NUMBER,
        STRING
};

enum class Token {
    CURLEY_OPEN,
    CURLEY_CLOSE,
    COLON,
    STRING,
    NUMBER,
    ARRAY_OPEN,
    ARRAY_CLOSE,
    COMMA,
    BOOLEAN,
    NULL_TYPE
}


#endif
