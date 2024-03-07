#include <string>

class BLEObject {
    private:
    std::string name;
    std::string path;
    std::string interface;

    public:
    void set_name(std::string name);
    void set_path(std::string path);
    void set_interface(std::string intrface);
    
    std::string get_name();
    std::string get_path();
    std::string get_interface();
};
