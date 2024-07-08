#ifndef JSON_NODE_H
#define JSON_NODE_H

//#include "using_vars.ho"
#include <map>
#include <vector>
#include <string>
#include <cstring>

namespace JSON {
    class JSONNode;
    using JSONObject = std::map<std::string, JSONNode*>;
    using JSONList = std::vector<JSONNode*>; 

    class JSONNode {

        enum class Type {
            OBJECT,
            LIST,
            STRING,
            BOOLEAN,
            NULL_TYPE,
            NUMBER
        } type;

        //Creates an object that contains one of these values
        //only takes the space of the largest value
        union Values {
            JSONObject *object;
            JSONList *list;
            std::string *s;
            float fVal;
            bool bVal;
        } values;

    public:
        void set_object(JSONObject* obj);
        void set_string(std::string str);
        void set_list(JSONList* list);
        void set_float(float val);
        void set_bool(bool val);

        JSON::JSONObject* get_object();
        std::string* get_string();
        JSON::JSONList* get_list();
        float get_float();
        bool get_bool();


    };

}

#endif
