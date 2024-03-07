#include <string>
#include <dbus-cxx.h>

class BLEObject {
    private:
    std::string path;
    std::string interface;
    std::shared_ptr<DBus::ObjectProxy> object;

    public:
    void set_path(std::string path);
    void set_interface(std::string intrface);
    void set_object(std::shared_ptr<DBus::ObjectProxy> object);
    
    std::string get_path();
    std::string get_interface();
    std::shared_ptr<DBus::ObjectProxy> get_object();
};
