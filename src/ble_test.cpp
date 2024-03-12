#include <dbus-cxx.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
//#include <parse_dbus.h>
#include <local_adapter.h>

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
std::shared_ptr<DBus::SignalProxy<void(std::string)>> listen_for_device_added(LocalAdapter object, std::shared_ptr<DBus::Connection> connection) {
/*
    std::shared_ptr<DBus::SignalProxy<void(std::string)>> signal = 
        connection->create_free_signal_proxy<void(std::string)>(
                DBus::MatchRuleBuilder::create()
                .set_path(object.get_path()) //path this devices adapter
                .set_interface(object.get_interface()) //That emits device added signal
                .set_member("InterfacesAdded") //signal to listen for
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread
                );
*/
    //Temporary signal test using hard coded paths
    std::shared_ptr<DBus::SignalProxy<void(std::string)>> signal = 
        connection->create_free_signal_proxy<void(std::string)>(
                DBus::MatchRuleBuilder::create()
                .set_path("/") //path this devices adapter
                .set_interface("org.freedesktop.DBus.ObjectManager") //That emits device added signal
                .set_member("InterfacesAdded") //signal to listen for
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread
                );
    //Create callback function to be called when signal is recieved
    signal->connect(sigc::ptr_fun(test_sig));

    std::cout << "Running" << std::flush;
    return signal;

    //for(int i = 0; i < 10; i++) {
        //std::cout << "." << std::flush;
        //sleep(1);
    //}
}

//create method proxies for StartDiscovery and StopDiscovery 
DBus::MethodProxy<void()>& create_scan_meth(std::shared_ptr<DBus::ObjectProxy> object, std::string interface, std::string function) {
    DBus::MethodProxy<void()>& funcProx = *(object->create_method<void()>(interface, function));
     return *(object->create_method<void()>(interface, function));

    //return funcProx;
}

//Change to use local adapter class
LocalAdapter extract_info(std::shared_ptr<DBus::ObjectProxy> object, BLEDeviceObject devObject, std::string base_path) {
    //Class to handle adapter information
    //LocalAdapter deviceAdapter;

    BLEDeviceObject::iterator it = devObject.begin();
    std::string pth;
    //Represents adapters the system has
    const std::string adapter = "org.bluez.Adapter1";

    //it->first is the path of the object
    //Im only looking for the first available bluetooth device, so if the path is longer than base_path return path as string
    while(it != devObject.end()) {
        pth = (std::string)it->first;
        //Represents the nested map in BLEDeviceObject containing interfaces
        BLEDeviceInterface::iterator itr = it->second.begin();

        std::cout << "Printing interfaces:\n";
        while(itr != it->second.end()) {
            if(itr->first == adapter) {
                LocalAdapter local(create_scan_meth(object, itr->first, "StartDiscovery"), create_scan_meth(object, itr->first, "StopDiscovery"));
                std::cout << "Adapter found: " << itr->first << "\n";
                //deviceAdapter.set_interface(itr->first);
                //deviceAdapter.set_path(pth);
                //deviceAdapter.create_adapter(object, pth, itr->first);
                return local;
            }
            itr++;
        }
        it++;
    }

    //Return object without method proxies if device is not found
    LocalAdapter local(create_scan_meth(object, "NULL", "NULL"), create_scan_meth(object, NULL, NULL));
    return local;
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

    //BLEObject ble_object = extract_info(answer, "/org/bluez");

    //LocalAdapter local = extract_info(answer, "/org/bluez");
    LocalAdapter local = extract_info(object, answer, "/org/bluez");
    //local.create_adapter(object, "/org/blues")
    local.start_scan();
    std::cout << "Enabling discovery\n";

    //listen_for_device_added(connection, ble_object);
    std::shared_ptr<DBus::SignalProxy<void(std::string)>> signal = listen_for_device_added(local, connection);
    
    for(int i = 0; i < 10; i++) {
        std::cout << "." << std::flush;
        sleep(1);
    }

    connection->remove_free_signal_proxy(signal);
    std::cout << "Testing after free\n";
    
    for(int i = 0; i < 10; i++) {
        std::cout << "." << std::flush;
        sleep(1);
    }
    
    local.stop_scan();

    return 0;
}
