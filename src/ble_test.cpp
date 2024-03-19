#include <dbus-cxx.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <local_adapter.h>
#include <found_ble.h>
/*
 * Create class that holds information found ble adatpers
 * Reformat this file to keep track of object interfaces as well as paths
 * Create method proxy to start and stop scanning for devcies
 */

    
//Holds return value of GetManagedObjects()
typedef std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> BLEDeviceObject;
//adapter for each path
typedef std::map<std::string, std::map<std::string, DBus::Variant>> BLEDeviceInterface;

FoundBLE get_interface_added(DBus::Path path, std::map<std::string, std::map<std::string, DBus::Variant>> other) {
    std::cout << "Signal " << path << " recieved\n";
    std::map<std::string, std::map<std::string, DBus::Variant>>::iterator it = other.begin();
    std::map<std::string, std::map<std::string, DBus::Variant>>::iterator itEnd = other.end();

    while(it != itEnd) {
        std::cout << "Map string: " << it->first << "\n";
      
        //Handles string, dict(string, dict{string, variant})
        if(it->second.size() > 0) {
            std::map<std::string, DBus::Variant>::iterator itr = it->second.begin();
            std::map<std::string, DBus::Variant>::iterator itrEnd = it->second.end();

            std::cout << "Adapter Name: " << it->second["Adapter"] << "\n";
            std::string address = it->second["Address"].to_string();
            std::cout << "Adapter Address: " << address << "\n" << std::flush; 
           
            std::cout << "UUID " << it->second["UUIDs"] << "\n" << std::flush;
            //std::vector<DBus::Variant> vect = it->second["UUIDs"].to_vector<DBus::Variant>();
            std::vector<std::string> vect = it->second["UUIDs"].to_vector<std::string>();

            //get the size of array holding UUIDs
            int UUIDCount = vect.size();

            if(UUIDCount > 0) {
                //Create object, signify that it is ble
                FoundBLE bleObj(1);
                for(int i = 0; i < UUIDCount; i++) {
                    std::cout << "UUID Found: " << vect[i] << "\n" << std::flush;
                    bleObj.add_UUID(vect[i]);

                }
                return bleObj;
            }
            else {
                FoundBLE bleObj(0);
                std::cout << "UUID not found\n";

                return bleObj;
            }

            std::cout << "Adapter Connected: " << it->second["Connected"].to_bool() << "\n" << std::flush;
            std::cout << "\n\n";

            }
        }
        std::cout << "\n";
        it++;

    }

    std::cout << "\n\n--------------------------\n\n";
}


void get_interface_removed(DBus::Path path, std::vector<std::string>) {
    std::cout << "Signal " << path << " removed\n";
}


//Listen for device added signal emmited on dbus
//massive function definition for creating signal to listen for interfaces added
std::shared_ptr<DBus::SignalProxy<void(DBus::Path,
        std::map<std::string, std::map<std::string, DBus::Variant>>)>> 
 listen_for_device_added(LocalAdapter object, std::shared_ptr<DBus::Connection> connection) {

     //Create a listener for the InterfacesAdded signal and call get_interface_added()
     std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>)>> signal = 
        connection->create_free_signal_proxy<void(DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>)>(
                DBus::MatchRuleBuilder::create()
                .set_path("/") //path this devices adapter
                .set_interface("org.freedesktop.DBus.ObjectManager") //That emits device added signal
                .set_member("InterfacesAdded") //signal to listen for
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread
                );
    //Create callback function to be called when signal is recieved
    signal->connect(sigc::ptr_fun(get_interface_added));

    std::cout << "Running\n" << std::flush;
    return signal;

}


std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> listen_for_device_removed(LocalAdapter object, 
        std::shared_ptr<DBus::Connection> connection) {
    //Create a listener for InterfacesRemoved signal and call get_interface_removed()
    std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> signal = 
        connection->create_free_signal_proxy<void(DBus::Path, std::vector<std::string>)>(
                DBus::MatchRuleBuilder::create()
                .set_path("/") //path this devices adapter
                .set_interface("org.freedesktop.DBus.ObjectManager") //That emits device added signal
                .set_member("InterfacesRemoved") //signal to listen for
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread
                );
    //Create callback function to be called when signal is recieved
    signal->connect(sigc::ptr_fun(get_interface_removed));

    std::cout << "Running\n" << std::flush;
    return signal;

}


//create method proxies for StartDiscovery and StopDiscovery 
DBus::MethodProxy<void()>& create_scan_meth(std::shared_ptr<DBus::ObjectProxy> object, std::string interface, std::string function) {
    DBus::MethodProxy<void()>& funcProx = *(object->create_method<void()>(interface, function));
     return *(object->create_method<void()>(interface, function));

    //return funcProx;
}

//Change to use local adapter class
LocalAdapter extract_info(std::shared_ptr<DBus::ObjectProxy> object, BLEDeviceObject devObject, std::string adapterPath) {
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
                //DBus::MethodProxy<void()>& startScanMeth = create_scan_meth(object, itr->first, "StartDiscovery");
                DBus::MethodProxy<void()>& startScanMeth = *(object->create_method<void()>(itr->first, "StartDiscovery"));
                std::cout << "Start sccan test\n";
                startScanMeth();

                LocalAdapter local(startScanMeth, create_scan_meth(object, itr->first, "StopDiscovery"));
                std::cout << "Adapter found: " << itr->first << "\n";
                return local;
            }
            itr++;
        }
        it++;
    }

    //Return object without method proxies if device is not found
    std::cout << "Adapter not found. Returning NULL\n";
    LocalAdapter local(create_scan_meth(object, "NULL", "NULL"), create_scan_meth(object, NULL, NULL));
    return local;
}


std::string get_local_adapter_path(BLEDeviceObject obj) {
    BLEDeviceObject::iterator it = obj.begin();
    BLEDeviceObject::iterator end = obj.end();
    std::cout << "Getting local adapter\n"; 
    std::string pth;
    //Represents adapters the system has
    const std::string adapter = "org.bluez.Adapter1";

    //loop through each object path
    while(it != end) {
        BLEDeviceInterface::iterator itr = it->second.begin(); //loop through nested map
        BLEDeviceInterface::iterator itrEnd= it->second.end(); //loop through nested map

        //loop through interfaces of each object. Return value should usually be /org/bluez/hci0
        while(itr != itrEnd) {
            if(itr->first == adapter) {
                pth = it->first;
                return pth;
            }
            itr++;
        }
        it++;
    }
    return "";
}


int main() {
    std::cout << "Creating dispatcher\n";
    //The dispatcher is what reads from and write to the bus
    std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();
 
    std::cout << "Creating connection\n";
    std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection(DBus::BusType::SYSTEM);

    std::cout << "Creating object\n";
    std::shared_ptr<DBus::ObjectProxy> baseObject = connection->create_object_proxy("org.bluez", "/");

    std::cout << "Creating proxy\n";
    //method returns a dict, which this library converts to a map
    //return consists of dict of {object path, dict of {string, dict of {string, variant}}}
    DBus::MethodProxy<BLEDeviceObject()>& method_proxy = *(baseObject->create_method<BLEDeviceObject()>("org.freedesktop.DBus.ObjectManager", "GetManagedObjects"));
    BLEDeviceObject answer;
    std::cout << "Getting Managed BLE Devices\n";
    answer = method_proxy();

    BLEDeviceObject::iterator it = answer.begin();
    
    //create new object for the found adapter
    //create if statment to check if exists
    std::string adapterPath = get_local_adapter_path(answer);

    //Make sure a ble device is found
    if(adapterPath != "") {
        std::cout << "test path: " << adapterPath << "\n";

        std::shared_ptr<DBus::ObjectProxy> adapterObject = connection->create_object_proxy("org.bluez", adapterPath);

        LocalAdapter local = extract_info(adapterObject, answer, it->first);
        
        std::cout << "Enabling discovery\n";
        local.start_scan();

        //create listener for device added
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, 
                std::map<std::string, std::map<std::string, DBus::Variant>>)>> addSignal =
            listen_for_device_added(local, connection);
       
        //Add reciever to listen for device removed signal
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> removeSignal = listen_for_device_removed(local, connection); 
       
        //do not continue for the moment
        while(true) {
            sleep(1);
        }

        //remove recievers when no longer needed
        connection->remove_free_signal_proxy(addSignal);
        connection->remove_free_signal_proxy(removeSignal);
        std::cout << "Testing after free\n";
        
        for(int i = 0; i < 10; i++) {
            std::cout << "." << std::flush;
            sleep(1);
        }
        
        local.stop_scan();
    }
    else {
        std::cout << "Bluetooth device not found\n";
    }

    return 0;
}
