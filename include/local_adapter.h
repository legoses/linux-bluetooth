#ifndef LOCAL_ADAPTER_H
#define LOCAL_ADAPTER_H

#include <parse_dbus.h>
#include <iostream>
#include <dbus-cxx.h>


class LocalAdapter : public BLEObject {
    DBus::MethodProxy<void()> start_scan_proxy;
    DBus::MethodProxy<void()> stop_scan_proxy;
    int currentlyScanning = 0;
    bool startSet = false;
    bool stopSet = false;

    public:
    LocalAdapter();
    LocalAdapter(DBus::MethodProxy<void()> &scanStart, DBus::MethodProxy<void()> &scanStop);

    void create_adapter(std::shared_ptr<DBus::ObjectProxy> object, std::string path, std::string interface);
    int start_scan();
    int stop_scan();

    void set_scan_start(DBus::MethodProxy<void()> &scanStart);
    void set_scan_stop(DBus::MethodProxy<void()> &scanStop);
};


#endif
