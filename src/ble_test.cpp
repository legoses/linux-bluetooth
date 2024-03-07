#include <dbus-cxx.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <parse_dbus.h>

/*
 * Create class that holds information found ble adatpers
 * Reformat this file to keep track of object interfaces as well as paths
 * Create method proxy to start and stop scanning for devcies
 */

//Holds return value of GetManagedObjects()
typedef std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> BLEDeviceObject;
//adapter for each path
typedef std::map<std::string, std::map<std::string, DBus::Variant>> BLEDeviceInterface;


void test_sig(std::string str) {
    std::cout << "Signal " << str << " recieved\n";
}


//Listen for device added signal emmited on dbus
void listen_for_device_added(BLEObject object, std::shared_ptr<DBus::Connection> connection) {
    std::shared_ptr<DBus::SignalProxy<void(std::string)>> signal = 
        connection->create_free_signal_proxy<void(std::string)>(
                DBus::MatchRuleBuilder::create()
                .set_path(object.get_path()) //path this devices adapter
                .set_interface(object.get_interface()) //That emits device added signal
                .set_member("InterfacesAdded") //signal to listen for
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread
                );

    //Create callback function to be called when signal is recieved
    signal->connect(sigc::ptr_fun(test_sig));

    std::cout << "Running" << std::flush;

    for(int i = 0; i < 10; i++) {
        std::cout << "." << std::flush;
        sleep(1);
    }
}


BLEObject extract_info(BLEDeviceObject object, std::string base_path) {
    //Class to handle adapter information
    BLEObject deviceAdapter;

    BLEDeviceObject::iterator it = object.begin();
    std::string pth;
    //Represents adapters the system has
    const std::string adapter = "org.bluez.Adapter1";

    //it->first is the path of the object
    //Im only looking for the first available bluetooth device, so if the path is longer than base_path return path as string
    while(it != object.end()) {
        pth = (std::string)it->first;

        //Represents the nested map in BLEDeviceObject containing interfaces
        BLEDeviceInterface::iterator itr = it->second.begin();

        std::cout << "Printing interfaces:\n";
        while(itr != it->second.end()) {
            if(itr->first == adapter) {
                std::cout << "Adapter found: " << itr->first << "\n";
                deviceAdapter.set_interface(itr->first);
                deviceAdapter.set_path(pth);
                return deviceAdapter;
            }
            itr++;
        }
        it++;
    }

    //return empty string if device is not found
    return deviceAdapter;
}


int main() {
    std::cout << "Creating dispatcher\n";
    //The dispatcher is what reads from and write to the bus
    std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();
 
    std::cout << "Creating connection\n";
    std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection(DBus::BusType::SYSTEM);

    std::cout << "Creating object\n";
    std::shared_ptr<DBus::ObjectProxy> object = connection->create_object_proxy("org.bluez", "/");

    std::cout << "Creating proxy\n";
    //method returns a dict, which this library converts to a map
    //return consists of dict of {object path, dict of {string, dict of {string, variant}}}
    DBus::MethodProxy<BLEDeviceObject()>& method_proxy = *(object->create_method<BLEDeviceObject()>("org.freedesktop.DBus.ObjectManager", "GetManagedObjects"));
    BLEDeviceObject answer;
    std::cout << "Answer size before: " << answer.size() << "\n";
    std::cout << "Calling method\n";
    answer = method_proxy();

    BLEDeviceObject::iterator it = answer.begin();
    //it++;
    //DBus::Path devPath = it->first;

    BLEObject ble_object = extract_info(answer, "/org/bluez");


    //listen_for_device_added(connection, ble_object);
    listen_for_device_added(ble_object, connection);

    return 0;
}
