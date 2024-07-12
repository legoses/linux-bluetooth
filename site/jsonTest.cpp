#include "json.h"

int main() {
    //uint8_t jsonArr[] = {'{', '"', 'A', 'd', 'd', 'r', 'e', 's', 's', '"', ':', '"', '4', 'C', ':', '7', '4', ':', '4', '4', ':', '4', '5', ':', '2', '2', ':', 'D', '9', '"', ',', '"', 'N', 'a', 'm', 'e', '"', ':', '"', 'D', 'N', 'E', '"', ',', '"', 'P', 'a', 't', 'h', '"', ':', '"', '/', 'o', 'r', 'g', '/', 'b', 'l', 'u', 'e', 'z', '/', 'h', 'c', 'i', '0', '/', 'd', 'e', 'v', '_', '4', 'C', '_', '7', '4', '_', '4', '4', '_', '4', '5', '_', '2', '2', '_', 'D', '9', '"', ',', '"', 'S', 'e', 'l', 'e', 'c', 't', 'e', 'd', '"', ':', '0', ',', '"', 't', 'e', 's', 't', '"', ':', 't', 'r', 'u', 'e', '}'};
    uint8_t jsonArr[] = {'{', '"', 'S', 'e', 'l', 'e', 'c', 't', 'e', 'd', '"', ':', '0', ',', '"', 'A', 'd', 'd', 'r', 'e', 's', 's', '"', ':', '"', '4', 'C', ':', '7', '4', ':', '4', '4', ':', '4', '5', ':', '2', '2', ':', 'D', '9', '"', ',', '"', 'N', 'a', 'm', 'e', '"', ':', '"', 'D', 'N', 'E', '"', ',', '"', 'P', 'a', 't', 'h', '"', ':', '"', '/', 'o', 'r', 'g', '/', 'b', 'l', 'u', 'e', 'z', '/', 'h', 'c', 'i', '0', '/', 'd', 'e', 'v', '_', '4', 'C', '_', '7', '4', '_', '4', '4', '_', '4', '5', '_', '2', '2', '_', 'D', '9', '"', ',', '"', 't', 'e', 's', 't', '"', ':', 't', 'r', 'u', 'e', ',', '"', 'a', 'r', 'r', 'a', 'y', '"', ':', '[', '"', 't', 'e', 's', 't', '"', ',', '"', 'o', 'n', 'e', '"', ',', '0', ',', '5', ',', ']', '}'};

    JsonObject obj(jsonArr, sizeof(jsonArr));
    obj.printArr();
    std::cout << "Get item test: " << obj.get_item("Selected")->get_float() << "\n";
    int i = obj.get_item("array")->get_list()->size();

    std::string numType;
    switch(obj.get_item("Selected")->get_type()) {
        case(JSON::Type::OBJECT):
            numType = "Object";
            break;
        case(JSON::Type::LIST):
            numType = "List";
            break;
        case(JSON::Type::STRING):
            numType = "string";
            break;
        case(JSON::Type::BOOLEAN):
            numType = "booll";
            break;
        case(JSON::Type::NULL_TYPE):
            numType = "null";
            break;
        case(JSON::Type::NUMBER):
            numType = "number";
            break;
    }
    std::cout << "type is: " << numType << "\n";
    std::cout << "to string test: " << obj.get_item("Selected")->to_string() << "\n";
    
    return 0;
}
