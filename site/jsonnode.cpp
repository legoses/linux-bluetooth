#include "jsonnode.h"


//delete appears to call deconstructior
JSON::JSONNode::~JSONNode() {
    switch(this->type) {
        case(JSON::Type::STRING):
            delete this->values.s;
            break;
        case(JSON::Type::LIST):
            free_list();
            break;
        case(JSON::Type::OBJECT):
            if(this->values.object != nullptr) {
                delete this->values.object;
                this->values.object = nullptr;
            }
            
            break;
    }
}


//remove any objects with allocated memory
void JSON::JSONNode::free_list() {
    for(int i = 0; i < this->values.list->size(); i++) {
        JSON::Type t = (*this->values.list)[i]->get_type();
        if((t == JSON::Type::STRING) || (t == JSON::Type::OBJECT) || (t == JSON::Type::LIST)) {
            delete (*this->values.list)[i];
        }
    }
    delete this->values.list;
    this->values.list = nullptr;
}

void JSON::JSONNode::set_object(JSON::JSONObject *obj) {
    //already initialized before this is called
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


std::string JSON::JSONNode::to_string() {
    switch(type) {
        case(Type::STRING): {
            return *this->values.s;
        }
        case(Type::BOOLEAN): {
            if(this->values.bVal) {
                return "true";
            }
            return "false";
        }
        case(Type::NULL_TYPE): {
            return "null";
        }
        case(Type::NUMBER): {
            return std::to_string(this->values.fVal);
        }
        default: {
            return "Unable to convert object to string";
        }
    }
}
