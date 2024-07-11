#include "jsonnode.h"

void JSON::JSONNode::set_object(JSON::JSONObject* obj) {
    this->values.object = obj;
}


void JSON::JSONNode::set_string(std::string str) {
    this->values.s = new std::string(str);
}


void JSON::JSONNode::set_list(JSONList* list) {
    this->values.list = list;
}


void JSON::JSONNode::set_float(float val) {
    this->values.fVal = val;
}


void JSON::JSONNode::set_bool(bool val) {
    this->values.bVal = val;
}


void JSON::JSONNode::set_type(JSON::Type t) {
    this->type = t;
}


JSON::JSONObject* JSON::JSONNode::get_object() {
    return this->values.object;
}


std::string* JSON::JSONNode::get_string() {
    return this->values.s;
}

JSON::JSONList* JSON::JSONNode::get_list() {
    return this->values.list;
}

float JSON::JSONNode::get_float() {
    return this->values.fVal;
}

bool JSON::JSONNode::get_bool() {
    return this->values.bVal;
}


JSON::Type JSON::JSONNode::get_type() {
    return this->type;
}
