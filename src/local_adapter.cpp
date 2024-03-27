#include <local_adapter.h>


LocalAdapter::LocalAdapter(DBus::MethodProxy<void()>& scanStart, DBus::MethodProxy<void()>& scanStop)
    : start_scan_proxy(scanStart)
    , stop_scan_proxy(scanStop) //initialize variables
{
}


void LocalAdapter::create_adapter(std::shared_ptr<DBus::ObjectProxy> object, std::string path, std::string interface) {
    //set name, interface path
    //create method proxies for scanning

    set_path(path);
    set_interface(interface);

    //create proxy objects
}


int LocalAdapter::start_scan() {
    std::cout << "Starting scan\n";
    //begin listening for discovery events
    start_scan_proxy();
    return 0;
}


int LocalAdapter::stop_scan() {
    std::cout << "Stopping scan\n";
    //begin listening for discovery events
    stop_scan_proxy();
    return 0;
}
