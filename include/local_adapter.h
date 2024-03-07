#include <parse_dbus.h>
#include <iostream>
#include <dbus-cxx.h>

class LocalAdapter : public BLEObject {
    DBus::MethodProxy<void()>& start_scan_proxy;
    DBus::MethodProxy<void()>& stop_scan_proxy;
    //DBus::MethodProxy<void()> start_scan_proxy;
    //DBus::MethodProxy<void()> stop_scan_proxy;
    int scan_start_proxy_set;
    int scan_stop_proxy_set; 


    public:
    LocalAdapter(DBus::MethodProxy<void()>& scanStart, DBus::MethodProxy<void()>& scanStop);
    void create_adapter(std::shared_ptr<DBus::ObjectProxy> object, std::string path, std::string interface);
    int start_scan();
    int stop_scan();
};
