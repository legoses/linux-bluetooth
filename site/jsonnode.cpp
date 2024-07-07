#include "jsonnode.h"

void JSON::JSONNode set_object(JSON::JSONObject* obj) {
    this->values.object = obj;
}


void JSON::JSONNode set_string(std::string &str) {
    this->values.s = str;
}

void JSON::JSONNode set_list(JSONList* list) {
    this->values.list = list;
}

void JSON::JSONNode set_float(float val) {
    this->values.fVal = val;
}

void JSON::JSONNode set_bool(bool val) {
    this->values.bVal = val;
}
