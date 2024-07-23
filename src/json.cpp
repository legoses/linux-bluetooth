#include <json.h>

JsonObject::JsonObject(uint8_t array[], int arrSize)
    : initArr{array}
    , initArrSize(arrSize)
    , tokenizer(array, arrSize) {
   parse(); 
}


/*
 * objects that need to be dealoocated
 * JSONNode
 * JSONObject
 * JSONList
 * Strings stored in json node
 */
JsonObject::~JsonObject() {
    std::cout << "Deleting json objects\n";
    JSON::JSONObject::iterator beg = this->list->begin();
    JSON::JSONObject::iterator end = this->list->end();
    while(beg != end) {
        std::cout << "Deleting: " << beg->first << "\n";
        delete beg->second;
        beg++;
    }
    delete this->list;
}

void JsonObject::print_output(const std::string &str) {
    if(this->debug) {
        std::cout << str << "\n";
    }
}


//if token is not in switch statment, will return a nullptr
JSON::JSONNode* JsonObject::detect_type(struct Token &token) {
    JSON::JSONNode *node = nullptr;
    switch(token.type) {
        case(TOKEN::CURLEY_OPEN): {
            node = parse_object();
            break;
        }
        case (TOKEN::STRING): {
            node = parse_string(token);
            print_output("recieved string " + token.c);

            break;
        }
        case (TOKEN::NUMBER): {
            node = parse_number(token);
            break;
        }
        case (TOKEN::BOOLEAN): {
            node = parse_boolean(token); 
            break;
        }
        case (TOKEN::NULL_TYPE): {
            JSON::JSONNode *node = new JSON::JSONNode();
            node->set_string("null");
            break;
        }
        case (TOKEN::ARRAY_OPEN): {
            node = parse_array();
            break;
        }
    }
    if(token.type == TOKEN::CURLEY_OPEN) {
        std::cout << "TEST: " << *(*node->get_object())["string"]->get_string() << "\n";

    }

    return node;
}


void JsonObject::printArr() {
    std::cout << this->initArr << "\n";
} 


void JsonObject::throw_error(bool a, std::string &expected) {
    if(a) {
        std::cout << "Error: Expected ',', got: " << expected << "\n";
    }
    else {
        std::cout << "Error: Expected ':', got: " << expected << "\n";
    }
}


//handle getting key, will always be string in my current use case
void JsonObject::parse() {
    struct Token token = tokenizer.get_token(); //key indicate if moving onto next key
    struct Token token2; //will hold value
    TOKEN lastSeperator = TOKEN::COMMA; //keep track of previous colon or comma
    
    while(this->validToken) {
        if(tokenizer.has_tokens()) {
            token = tokenizer.get_token(); //should be key
            
            if(token.type == TOKEN::COMMA) {
                token = tokenizer.get_token(); //should be key
                token2 = tokenizer.get_token(); //should be key
            }

            token2 = tokenizer.get_token(); //should be a colon


            if(token2.type == TOKEN::COLON && lastSeperator == TOKEN::COMMA) {
                lastSeperator = token2.type;
                token2 = tokenizer.get_token(); //get value
                JSON::JSONNode *n = detect_type(token2);
            
                (*this->list)[token.c] = n;

                if(n->get_type() == JSON::Type::OBJECT) {
                    std::cout << "TEST2: " << *(*n->get_object())["string"]->get_string() << "\n";

                }
                print_output("Added key: " + token.c + " value: " + token2.c);


                //if a comma is not detected, end of json
                if(tokenizer.get_token().type == TOKEN::COMMA) {
                    lastSeperator = TOKEN::COMMA; //reset to comma
                }
                else {
                    this->validToken = false; //if a comma was not detected, end of JSON
                }
            }
            else {
                this->validToken = false; //stop execution if colon is not detected
                throw_error(false, token2.c);
            }
        }
        else {
            this->validToken = false; //end execution if no more tokens
        }
    }

    if(lastSeperator == TOKEN::COMMA) {
        std::cout << "Warning: Stray comma detected\n";
    }
}


JSON::JSONNode* JsonObject::parse_number(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    node->set_float(std::stof(token.c));
    node->set_type(JSON::Type::NUMBER);

    return node;
}


JSON::JSONNode* JsonObject::parse_string(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    node->set_string(token.c);
    node->set_type(JSON::Type::STRING);

    //string must be deleted on deconstruction
    return node; 
}


JSON::JSONNode* JsonObject::parse_boolean(struct Token &token) {
    JSON::JSONNode *node = new JSON::JSONNode();
    if(token.c[0] == 't') {
        node->set_bool(true);
    }
    else if(token.c[0] == 'f') {
        node->set_bool(false);
    }
    node->set_type(JSON::Type::BOOLEAN);

    return node;
}


JSON::JSONNode* JsonObject::parse_object() {
    std::cout << "Parse object called\n";
    JSON::JSONObject *obj = new JSON::JSONObject();
    JSON::JSONNode *node = new JSON::JSONNode();

    struct Token token;// = tokenizer.get_token(); //key indicate if moving onto next key
    struct Token token2; //will hold value
    TOKEN lastSeperator = TOKEN::COMMA; //keep track of previous colon or comma
    
    while(this->validToken) {
        if(tokenizer.has_tokens()) {
            token = tokenizer.get_token(); //should be key
            
            if(token.type == TOKEN::COMMA) {
                token = tokenizer.get_token(); //should be key
                token2 = tokenizer.get_token(); //should be key
            }
           
            token2 = tokenizer.get_token(); //should be a colon


            if(token2.type == TOKEN::COLON && lastSeperator == TOKEN::COMMA) {
                lastSeperator = token2.type;
                token2 = tokenizer.get_token(); //get value
            
                (*obj)[token.c] = detect_type(token2); //add to list
                print_output("Added key: " + token.c + " value: " + token2.c);

                //if a comma is not detected, end of json
                if(tokenizer.get_token().type == TOKEN::COMMA) {
                    lastSeperator = TOKEN::COMMA; //reset to comma
                }
                else {
                    this->validToken = false; //if a comma was not detected, end of JSON
                }
            }
            else {
                this->validToken = false; //stop execution if colon is not detected
                throw_error(false, token2.c);
            }
        }
        else {
            this->validToken = false; //end execution if no more tokens
        }
    }

    if(lastSeperator == TOKEN::COMMA) {
        std::cout << "Warning: Stray comma detected B\n";
    }

    node->set_object(obj);
    node->set_type(JSON::Type::OBJECT);
    
    return node;
}


//make sure array closes before a stray } is detected, and last value does not have a ,
JSON::JSONNode* JsonObject::parse_array() {
    JSON::JSONNode *node = new JSON::JSONNode(); //will be parent node to store list
    JSON::JSONList *lst = new JSON::JSONList();
    bool cont = true;

    struct Token token; 
    print_output("Array called");
   
    while(cont) {
        cont = false;
        if(!tokenizer.has_tokens()) {
            break;
        }

        token = tokenizer.get_token();
        print_output("List token: " + token.c);
        if(token.type == TOKEN::ARRAY_CLOSE) {
            std::cout << "Warning: Stray comma detected\n";
            break;
        }

        lst->push_back(detect_type(token));
        
        token = tokenizer.get_token();

        switch(token.type) {
            case(TOKEN::ARRAY_CLOSE): {
                print_output("ARRAY CLOSED");
                cont = false;
                break;
            }
            case(TOKEN::CURLEY_CLOSE): {
                cont = false;
                std::cout << "Error: Json end detected before array close\n";
                break;
            }
            case(TOKEN::COMMA): {
                print_output("comma hara");
                cont = true;
                break;
            }
        }
    }
   
    print_output("Current list token " + token.c);
    node->set_list(lst);
    node->set_type(JSON::Type::LIST);
    return node;
}


JSON::JSONNode* JsonObject::get_item(const std::string &key) {
    if((*this->list).find(key) != (*this->list).end()) {
        return (*this->list)[key];
    }
    return nullptr;
}

