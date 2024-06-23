#include <dbus-cxx.h>
//#include <unistd.h>
//#include <iostream>
#include <string>
//#include <vector>
//#include <map>
#include <local_adapter.h>
#include <found_ble.h>
#include <websocket.h>
//#include <encode.h>
/*
 * TODO:
 * Create a public websocket function to queue messages to send to websocket client
 */

    
//Holds return value of GetManagedObjects()
typedef std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> BLEDeviceObject;
//adapter for each path
typedef std::map<std::string, std::map<std::string, DBus::Variant>> BLEDeviceInterface;


bool device_exists(std::vector<FoundBLE> &knownBleObj, std::string &address) {
    for(FoundBLE obj : knownBleObj) {
        std::cout << "comparing " << address << " to " << obj.get_attribute("Address") << "\n";
        if(address != obj.get_attribute("Address")) {
            return true;
        }
        
    }
    return false;
}

//This is what currently parses scanned devices
//Create a class to handle this
void get_interface_added(DBus::Path path, BLEDeviceInterface other, std::vector<FoundBLE> &knownBleObj, std::mutex &mtx) {
    std::cout << "Device at: " << path << " found\n";
    std::map<std::string, std::map<std::string, DBus::Variant>>::iterator it = other.begin();
    std::map<std::string, std::map<std::string, DBus::Variant>>::iterator itEnd = other.end();

    while(it != itEnd) {
        std::string mapString = it->first;
        //std::cout << "Interface: " << mapString << "\n";
      
        //Handles string, dict(string, dict{string, variant})
        if(it->second.size() > 0 && mapString == "org.bluez.Device1") {
            std::map<std::string, DBus::Variant>::iterator itr = it->second.begin();
            std::map<std::string, DBus::Variant>::iterator itrEnd = it->second.end();
            
            //Crash happens after this line
            //std::cout << "Adapter Name: " << it->second["Adapter"] << "\n";
            std::string address = it->second["Address"];
            //std::cout << "Adapter Address: " << address << "\n" << std::flush; 
           
            std::vector<std::string> vect = it->second["UUIDs"].to_vector<std::string>();

            //get the size of array holding UUIDs
            int UUIDCount = vect.size();

            //if(!device_exists(knownBleObj, address) && it->second["Paired"].to_bool() == false) {
            if(it->second["Paired"].to_bool() == false) {
                if(UUIDCount > 0) {

                    //Create object, signify that it is ble
                    //Possible swtich to pointer
                    //for(int i = 0; i < UUIDCount; i++) {
                        //std::cout << "UUID Found: " << vect[i] << "\n" << std::flush;
                    //    bleObj.add_UUID(vect[i]);
                    //}
                    
                    FoundBLE bleObj(1);
                    bleObj.add_vect("UUID", vect);
                    bleObj.add_value("Path", path);
                    bleObj.add_value("Address", it->second["Address"].to_string());
               
                    //Add name if one exists. Otherwise add DNE
                    //maybe add checks like this for other attributes as well?
                    if(it->second["Name"].type() == DBus::DataType::INVALID) {
                        bleObj.add_value("Name", "DNE");
                    }
                    else {
                        bleObj.add_value("Name", it->second["Name"].to_string());
                    }

                    std::cout << "Adding device\n";
                    mtx.lock();
                    knownBleObj.push_back(bleObj);
                    mtx.unlock();
                    //std::cout << "test size: " << knownBleObj.size() << "\n";

                }
                else {
                    FoundBLE bleObj(0);
                    
                    if(it->second["Name"].type() == DBus::DataType::INVALID) {
                        bleObj.add_value("Name", "DNE");
                    }
                    else {
                        bleObj.add_value("Name", it->second["Name"].to_string());
                    }
                    bleObj.add_value("Path", path);
                    bleObj.add_value("Address", it->second["Address"].to_string());
                
                    std::cout << "Adding device\n";
                    mtx.lock();
                    knownBleObj.push_back(bleObj);
                    mtx.unlock();
                }
            }
            else {
                std::cout << "UUID not found or device exists\n";
            }
            
        }
        ++it;
    }
}


void get_interface_removed(DBus::Path path, std::vector<std::string>, std::vector<FoundBLE> &knownBleObj, std::mutex &mtx) {
    //convert path to string to it can be compared to stored object path
    std::string pathStr = path;
    
    //Check for race conditions later
    //remove device that is no longer seen
    mtx.lock();
    //iterator is used because the erase function requires this instead of an index number
    for(std::vector<FoundBLE>::iterator it = knownBleObj.begin(); it != knownBleObj.end(); ++it) {
        //check dbus path against object
        if(pathStr == it->get_attribute("Path")) {
           
            knownBleObj.erase(it);
            
            std::cout << "Signal " << path << " removed\n";
            //dont forget to unlock mutex if a signal is removed
            mtx.unlock();
            return;
        }
    }
    mtx.unlock();

    std::cout << "Error: Tried to remove device at path: '" << path << "' but device was not found.\n";
}


//Listen for device added signal emmited on dbus
//massive function definition for creating signal to listen for interfaces added
std::shared_ptr<DBus::SignalProxy<void(DBus::Path, BLEDeviceInterface)>> listen_for_device_added(std::shared_ptr<DBus::Connection> connection, std::vector<FoundBLE> &foundBleObj, std::mutex &mtx) {
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
    signal->connect(sigc::bind(sigc::ptr_fun(&get_interface_added), std::ref(foundBleObj), std::ref(mtx)));

    std::cout << "Running\n" << std::flush;
    return signal;
}


//listen for devices removed signal
std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> listen_for_device_removed(
            std::shared_ptr<DBus::Connection> connection,
            std::vector<FoundBLE> &foundBleObj,
            std::mutex &mtx) {
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
    signal->connect(sigc::bind(sigc::ptr_fun(get_interface_removed), std::ref(foundBleObj), std::ref(mtx)));

    std::cout << "Running\n" << std::flush;
    return signal;
}


//create method proxies for StartDiscovery and StopDiscovery 
DBus::MethodProxy<void()>& create_scan_meth(std::shared_ptr<DBus::ObjectProxy> &object, std::string interface, std::string function) {
     return *(object->create_method<void()>(interface, function));
}


int uint_to_int(uint8_t num) {
    return (int)num - '0';
}


//Creats an object to iteract with the ble adapter on this device
LocalAdapter parse_known_devices(std::shared_ptr<DBus::Connection> connection, BLEDeviceObject &devices, std::vector<FoundBLE> &knownBleDevices, std::mutex &mtx) {
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
                get_interface_added(it->first, it->second, knownBleDevices, mtx);
                break;
            }
            ++itr;
        }
        ++it;
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
    std::cout << "ERROR: No device found. Null proxies created\n";
    DBus::MethodProxy<void()> &scanStart = *(adapterObject->create_method<void()>("null", "null"));
    DBus::MethodProxy<void()> &scanStop = *(adapterObject->create_method<void()>("null", "null"));

    LocalAdapter adapter(scanStart, scanStop);

    return adapter;
}


//signal how many deviecs will be sent
void signal_amount_of_devices(Web::WebsocketServer &server, int &size) {
    //char sizeArr[] = {'{', '"', 'd', 'e', 'v', 'i', 'c', 'e', 's', '"', ':', '"', '0', '"', '}', '\0'};
    char begSizeArr[] = {'{', '"', 'd', 'e', 'v', 'i', 'c', 'e', 's', '"', ':', '"'};
    char endSizeArr[] = {'"', '}'};
    //if double digit, needs to be broken into individual characters?
    //std::cout << sizeArr << "\n";
    //server.send_data(sizeArr, (sizeof(sizeArr)/sizeof(char))-1);
    
    
    server.send_data(begSizeArr, sizeof(begSizeArr)/sizeof(char), false);
    if(size > 9) {
        char *size1, *size2;// = size/10;
        size1 = (char*)(size/10);
        size2 = (char*)(size%10);
        std::cout << "division test: " << size1 << " " << size2 << "\n";
        server.send_data(size1, 1, false);
        server.send_data(size2, 1, false);
    }
    else {
        char* s = (char*)size;
        server.send_data(s, 1, false);
    }
    server.send_data(endSizeArr, sizeof(endSizeArr)/sizeof(char), true);
}


//loop through ble devices to prepare to send to websocket client
void send_ble_devices(std::vector<FoundBLE> &knownBleDevices, Web::WebsocketServer &server, std::mutex &mtx) {
    if(knownBleDevices.size() > 0) {
        char jsonStr[1024];
        std::cout << "Sending objects to websocket client\n\n";
        //make sure to have a way to deal with theads MUTEX
        //potentially chcange to while loop. only use mutex while accessing an object to minimize time
        //the vector in unavailable
        mtx.lock();
        int numKnownDevices = knownBleDevices.size();
        signal_amount_of_devices(server, numKnownDevices);
        for(int i = 0; i < numKnownDevices; i++) {
            //only apply lock while item is being accessed
            //make sure index has not gone out of range since beginning of iteration
            int jsonSize = knownBleDevices[i].obj_json(jsonStr, 1024);

            server.send_data(jsonStr, jsonSize, true);
        }
        mtx.unlock();
    }
}


int main() {
    //allocate memory for pointer vector
    std::vector<FoundBLE> knownBleDevices;

    std::mutex mtx;

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
    LocalAdapter local = parse_known_devices(connection, answer, knownBleDevices, mtx);

    //parse info from answer. Get local adapter object 
    std::cout << "Get path test: " << local.get_path() << "\n";

    //Make sure a ble device is found
    if(local.get_path() != "") {
        std::shared_ptr<DBus::ObjectProxy> adapterObject = connection->create_object_proxy("org.bluez", local.get_path());

        //create listener for device added
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, BLEDeviceInterface)>> addSignal = listen_for_device_added(connection, knownBleDevices, mtx);
       
        //Add reciever to listen for device removed signal
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> removeSignal = listen_for_device_removed(connection, knownBleDevices, mtx); 

        //create variable to hold commands from websocket site
        //uint8_t cmd = 0;
        Web::WebsocketServer server(8080);

        //lambda callback for websocket class
        server.set_threading(true);

        std::cout << "Starting webserver\n";
        server.begin();
        sleep(1);

        //listen for websocket events
        int cmd = uint_to_int(server.get_command());
        bool check_cmd = true;
        while(true) {
            const int scanTime = 10;
            const int waitTime = 15; //rescan before bluetooth gets depopulated

            //signal if command need to be checked, or if it has been set by previous loop
            if(check_cmd) {
                cmd = uint_to_int(server.get_command());
            }
            else {
                check_cmd = true;
            }


            if(cmd != 0) {
                std::cout << "COMMAND: " << cmd << "\n";
            }
            switch(cmd) {
                case 1: {
                    //start initial scan. use while loop to start and stop later
                    local.start_scan();
                    int startTime = time(nullptr);
                    //scan for 10 seconds, wait for 10 seconds
                    bool scan = true;
                    //continue until stop scanning is explicitly sent
                    while(cmd <= 1) {
                        int scanWait = time(nullptr) - startTime;
                        //start scan if not currently scanning, and waited longer than 30 seconds
                        if(!scan && scanWait >= waitTime) {
                            mtx.lock();
                            local.start_scan();
                            mtx.unlock();
                            scan = true;
                            startTime = time(nullptr);
                        }
                        else if(scan && scanWait >= scanTime) { //stop scan if currently scanning and waited longer than to seconds
                            mtx.lock();
                            local.stop_scan();
                            mtx.unlock();
                            send_ble_devices(knownBleDevices, server, mtx);
                            scan = false;
                            startTime = time(nullptr);
                        }
                        sleep(1);
                        cmd = uint_to_int(server.get_command());
                    }
                    check_cmd = false;
                    break;
                }
                case 2: {
                    mtx.lock();
                    local.stop_scan();
                    mtx.unlock();
                    std::cout << "Scan explicitly stopped\n";
                    
                    send_ble_devices(knownBleDevices, server, mtx);
                    break;
                }
            }
            sleep(.1);
        }

        //remove recievers when no longer needed
        connection->remove_free_signal_proxy(addSignal);
        connection->remove_free_signal_proxy(removeSignal);
        std::cout << "Testing after free\n";
    }
    else {
        std::cout << "Bluetooth device not found\n";
    }

    return 0;
}
