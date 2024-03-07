#include <parse_dbus.h>
#include <iostream>

class LocalAdapter : public BLEObject {
    DBus::MethodProxy<void()>& start_scan_proxy = NULL;
    DBus::MethodProxy<void()>& stop_scan_proxy = NULL;

    public:
    void create_adapter(std::shared_ptr<DBus::ObjectProxy> object, std::string path, std::string interface);
};
