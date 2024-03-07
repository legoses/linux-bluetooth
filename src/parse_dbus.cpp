#include <parse_dbus.h>

void BLEObject::set_name(std::string name) {
    this->name = name;
}


void BLEObject::set_path(std::string path) {
    this->path = path;
}


void BLEObject::set_interface(std::string interface) {
    this->interface = interface;
}


std::string BLEObject::get_name() {
    return this->name;
}


std::string BLEObject::get_path() {
    return this->path;
}


std::string BLEObject::get_interface() {
    return this->interface;
}
