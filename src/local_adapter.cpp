#include <local_adapter.h>

void LocalAdapter::create_adapter(std::shared_ptr<DBus::ObjectProxy> object, std::string path, std::string interface) {
    //set name, interface path
    //create method proxies for scanning

    set_path(path);
    set_interface(interface);

    //create proxy objects
    this->start_scan_proxy = *(object->create_method<void()>(interface, "StartDiscovery"));
    this->stop_scan_proxy = *(object->create_method<void()>(interface, "StopDiscovery"));
}
