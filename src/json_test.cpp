#include <json.h>


void create_json() {
    char jsonArr[] = {'{', '"', 't', 'e', 's', 't', '"', ':', ' ', 't', 'r', 'u', 'e', ',', '"', 'h', 'e', 'l', 'l', 'o', '"', ':', ' ', '"', 'e', 'h', '"', ',', '"', 'a', 'r', 'r', 'a', 'y', '"', ':', ' ', '[', '"', 'f', 's', 'd', 'f', '"', ',', ' ', '6', ',', ' ', '"', 'd', 'd', 'd', '"', ',', ' ', 'f', 'a', 'l', 's', 'e', ']', ',', '"', 'n', 'u', 'm', 'b', 'e', 'r', 'h', 'e', 'r', 'e', 'r', '"', ':', ' ', '6', ',', '"', 'b', 'e', 'w', 'i', 'o', 'b', 'e', 'j', 'c', 't', '"', ':', ' ', '{', '"', 's', 't', 'r', 'i', 'n', 'g', '"', ':', ' ', '"', 'f', 'd', 's', '"', ',', '"', 'n', 'u', 'm', 'b', 'e', 'r', '2', '"', ':', ' ', '2', ',', '"', 'l', 'i', 's', 't', 's', '"', ':', ' ', '[', 'f', 'a', 'l', 's', 'e', ',', ' ', 'f', 'a', 'l', 's', 'e', ']', '}', '}'};


    JsonObect obj(jsonArr, sizeof(jsonArr)/sizeof(char));

    std::cout << "test: " << obj.get_item("hello") << "\n";
}

int main() {
    create_json();

    return 0;
}
