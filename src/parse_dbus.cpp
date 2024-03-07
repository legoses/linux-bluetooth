#include <parse_dbus.h>


void BLEObject::set_path(std::string path) {
    this->path = path;
}


void BLEObject::set_interface(std::string interface) {
    this->interface = interface;
}



std::string BLEObject::get_path() {
    return this->path;
}


std::string BLEObject::get_interface() {
    return this->interface;
}


void BLEObject::set_object(std::shared_ptr<DBus::ObjectProxy> object) {
    this->object = object;
}


std::shared_ptr<DBus::ObjectProxy> BLEObject::get_object() {
    return this->object;
}
