#include <dbus-cxx.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <local_adapter.h>
#include <found_ble.h>
/*
 * TODO:
 * Figure out what do actually do with this program
 */

    
//Holds return value of GetManagedObjects()
typedef std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> BLEDeviceObject;
//adapter for each path
typedef std::map<std::string, std::map<std::string, DBus::Variant>> BLEDeviceInterface;


void get_interface_added(DBus::Path path, BLEDeviceInterface other, std::vector<FoundBLE> &knownBleObj) {
    std::cout << "Device at: " << path << " found\n";
    std::map<std::string, std::map<std::string, DBus::Variant>>::iterator it = other.begin();
    std::map<std::string, std::map<std::string, DBus::Variant>>::iterator itEnd = other.end();

    while(it != itEnd) {
        std::string mapString = it->first;
        std::cout << "Interface: " << mapString << "\n";
      
        //Handles string, dict(string, dict{string, variant})
        if(it->second.size() > 0 && mapString == "org.bluez.Device1") {
            std::map<std::string, DBus::Variant>::iterator itr = it->second.begin();
            std::map<std::string, DBus::Variant>::iterator itrEnd = it->second.end();

            //Crash happens after this line
            std::cout << "Adapter Name: " << it->second["Adapter"] << "\n";
            std::string address = it->second["Address"];
            std::cout << "Adapter Address: " << address << "\n" << std::flush; 
           
            std::vector<std::string> vect = it->second["UUIDs"].to_vector<std::string>();

            //get the size of array holding UUIDs
            int UUIDCount = vect.size();

            std::cout << "Adding device\n";
            if(UUIDCount > 0) {
                //Create object, signify that it is ble
                FoundBLE bleObj(1);
                for(int i = 0; i < UUIDCount; i++) {
                    std::cout << "UUID Found: " << vect[i] << "\n" << std::flush;
                    bleObj.add_UUID(vect[i]);
                }


                bleObj.set_path(path);
                knownBleObj.push_back(bleObj);
                std::cout << "test size: " << knownBleObj.size() << "\n";
            }
            else {
                FoundBLE bleObj(0);
                std::cout << "UUID not found\n";
                bleObj.set_path(path);
            
                knownBleObj.push_back(bleObj);
            }
            std::cout << "\n\n";

            
        }
        std::cout << "\n";
        it++;

    }

    std::cout << "\n\n--------------------------\n\n";
}


void get_interface_removed(DBus::Path path, std::vector<std::string>, std::vector<FoundBLE> &knownBleObj) {
    //convert path to string to it can be compared to stored object path
    std::string pathStr = path;

    //Check for race conditions later
    //remove device that is no longer seen
    for(int i = 0; i < knownBleObj.size(); i++) {
        if(pathStr == knownBleObj[i].get_path()) {
            //create iterator based on current
            std::vector<FoundBLE>::iterator it = knownBleObj.begin() + i;

            //delete knownBleObj[i];
            
            knownBleObj.erase(it);
            std::cout << "Signal " << path << " removed\n";
            return;
        }
    }

    std::cout << "Error: Tried to remove device at path: '" << path << "' but device was not found.\n";
}


//Listen for device added signal emmited on dbus
//massive function definition for creating signal to listen for interfaces added
std::shared_ptr<DBus::SignalProxy<void(DBus::Path, BLEDeviceInterface)>> listen_for_device_added(LocalAdapter object, std::shared_ptr<DBus::Connection> connection, std::vector<FoundBLE> &foundBleObj) {
     //Create a listener for the InterfacesAdded signal and call get_interface_added()
     std::shared_ptr<DBus::SignalProxy<void(DBus::Path, BLEDeviceInterface)>> signal = connection->create_free_signal_proxy<void(DBus::Path, BLEDeviceInterface)>(
                    DBus::MatchRuleBuilder::create()
                    .set_path("/") //path this devices adapter
                    .set_interface("org.freedesktop.DBus.ObjectManager") //That emits device added signal
                    .set_member("InterfacesAdded") //signal to listen for
                    .as_signal_match(),
                    DBus::ThreadForCalling::DispatcherThread);
    //Create callback function to be called when signal is recieved
    //sigc::bind allows me to pass an additional argument
    //std::ref must be used, otherwise a copy of vector will be passed, instead of a reference
    signal->connect(sigc::bind(sigc::ptr_fun(&get_interface_added), std::ref(foundBleObj)));


    std::cout << "Running\n" << std::flush;
    return signal;
}


//listen for devices removed signal
std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> listen_for_device_removed(
            LocalAdapter object, 
            std::shared_ptr<DBus::Connection> connection,
            std::vector<FoundBLE> &foundBleObj) {
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
    signal->connect(sigc::bind(sigc::ptr_fun(get_interface_removed), std::ref(foundBleObj)));

    std::cout << "Running\n" << std::flush;
    return signal;
}


//create method proxies for StartDiscovery and StopDiscovery 
DBus::MethodProxy<void()>& create_scan_meth(std::shared_ptr<DBus::ObjectProxy> &object, std::string interface, std::string function) {
     return *(object->create_method<void()>(interface, function));
}


LocalAdapter parse_known_devices(std::shared_ptr<DBus::Connection> connection, BLEDeviceObject &devices, std::vector<FoundBLE> &knownBleDevices) {
    BLEDeviceObject::iterator it = devices.begin();
    BLEDeviceObject::iterator itEnd = devices.end();

    //Initialize to empty path
    //this requires other values to be initialized
    std::string pth = "";
    std::shared_ptr<DBus::ObjectProxy> adapterObject = NULL;

    //ble adapter of local device
    std::string adapterName = "org.bluez.Adapter1"; 
    //Iterate through devices, check if device is adapter or not
    while(it != itEnd) {
        BLEDeviceInterface::iterator itr = it->second.begin();
        BLEDeviceInterface::iterator itrEnd = it->second.end();
        std::cout << "adapter made with path: " << pth << "\n";
        adapterObject = connection->create_object_proxy("org.bluez", pth);

        while(itr != itrEnd) {
            if(itr->first == adapterName) {
                pth = (std::string) it->first;
                std::cout << "Adapter found\n";
                break;
            }
            else if(itr->first == "org.bluez.Device1") {
                get_interface_added(it->first, it->second, knownBleDevices);
                break;
            }
            itr++;
        }
        it++;
    }
    //Create class with ability to start and stop scan
    if(adapterObject != NULL) {
        std::cout << "Creating scan method proxies\n";
        DBus::MethodProxy<void()> &scanStart = *(adapterObject->create_method<void()>("org.bluez.Adapter1", "StartDiscovery"));
        DBus::MethodProxy<void()> &scanStop = *(adapterObject->create_method<void()>("org.bluez.Adapter1", "StopDiscovery"));
        
        LocalAdapter adapter(scanStart, scanStop);
        adapter.set_path(pth);
        return adapter;
    }

    sleep(1);
    DBus::MethodProxy<void()> &scanStart = *(adapterObject->create_method<void()>("null", "null"));
    DBus::MethodProxy<void()> &scanStop = *(adapterObject->create_method<void()>("null", "null"));

    scanStart();
    while(true) {
        std::cout << "testing scn start\n";
        sleep(1);
    }

    LocalAdapter adapter(scanStart, scanStop);

    return adapter;
}


int main() {
    //allocate memory for pointer vector
    std::vector<FoundBLE> knownBleDevices;

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
    BLEDeviceObject answer = method_proxy();
    LocalAdapter local = parse_known_devices(connection, answer, knownBleDevices);

    //parse info from answer. Get local adapter object 
    std::cout << "Get path test: " << local.get_path() << "\n";

    //Make sure a ble device is found
    if(local.get_path() != "") {
        std::shared_ptr<DBus::ObjectProxy> adapterObject = connection->create_object_proxy("org.bluez", local.get_path());

        //create listener for device added
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, BLEDeviceInterface)>> addSignal = listen_for_device_added(local, connection, knownBleDevices);
       
        //Add reciever to listen for device removed signal
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> removeSignal = listen_for_device_removed(local, connection, knownBleDevices); 

        local.start_scan();
      
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
