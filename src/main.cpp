#include <dbus-cxx.h>
#include <string>
#include <local_adapter.h>
#include <found_ble.h>
#include <websocket.h>
#include <json.h>

typedef std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> BLEDeviceObject;
typedef std::map<std::string, std::map<std::string, DBus::Variant>> BLEDeviceInterface;
const int ATTACK_THREADS = 4;


LocalAdapter parse_known_devices(std::shared_ptr<DBus::Connection> connection, BLEDeviceObject &devices, Web::WebsocketServer &server) {
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
                //check_for_device(it->first, it->second, knownBleDevices, mtx, server);
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


//check if recieved device is already known
bool check_for_device(std::string &str, std::vector<std::string> &vect) {
    for(int i = 0; i < vect.size(); i++) {
        if(vect[i] == str) {
            return true;
        }
    }

    return false;
}


//prepare json to send to websocket site
std::string prepare_json_add(BLEDeviceInterface &device, std::string &path) {
    std::map<std::string, DBus::Variant> properties = device["org.bluez.Device1"];
   
    std::string jsonStr = "{\"Name\":\"" + properties["Name"].to_string() + 
        "\",\"Address\":\"" + properties["Address"].to_string() + 
        "\",\"Operation\":" + std::to_string(1) + 
        ",\"Path\":\"" + path + "\"}";
    std::cout << "add jsonstr test: " << jsonStr << "\n";
    return jsonStr;
}


std::string prepare_json_remove(std::string &path) {
    std::string jsonStr = "{\"Path\":\"" + path + "\",\"Operation\":" + std::to_string(0) + "}";
    std::cout << "Removign jsonstr test: " << jsonStr << "\n";
    return jsonStr;
}


void device_add(DBus::Path path, BLEDeviceInterface device, Web::WebsocketServer &server) {
    std::cout << "Device added at path: " << device["Address"] << "\n";
    std::string jsonStr = prepare_json_add(device, path);
    char data[jsonStr.size()];
    memcpy(data, jsonStr.c_str(), jsonStr.size());

    //server.send_data(tst, sizeof(tst)/sizeof(char), false);
    server.send_data(data, jsonStr.size(), false);
}


void device_remove(DBus::Path path, std::vector<std::string> devices, Web::WebsocketServer &server) {
    std::cout << "Device removed at path: " << path << "\n";

    std::string jsonStr = prepare_json_remove(path);
    char data[jsonStr.size()];
    memcpy(data, jsonStr.c_str(), jsonStr.size());

    server.send_data(data, jsonStr.size(), false);
}


std::shared_ptr<DBus::SignalProxy<void(DBus::Path, BLEDeviceInterface)>> listen_for_device_added(std::shared_ptr<DBus::Connection> &connection, Web::WebsocketServer &server) {
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
    signal->connect(sigc::bind(sigc::ptr_fun(&device_add), std::ref(server)));

    std::cout << "Running\n" << std::flush;
    return signal;
}


std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> listen_for_device_removed(
            std::shared_ptr<DBus::Connection> &connection,
            Web::WebsocketServer &server) {
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
    signal->connect(sigc::bind(sigc::ptr_fun(&device_remove), std::ref(server)));

    std::cout << "Running\n" << std::flush;
    return signal;
}


int main() {
    //create variable to hold commands from websocket site
    Web::WebsocketServer server(8080);

    //lambda callback for websocket class
    server.set_threading(true);
    int bufSize = 2000;
    uint8_t buf[bufSize];
    server.set_buffer(bufSize);

    std::cout << "Starting webserver\n";
    server.begin();
    sleep(1);

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
    LocalAdapter local = parse_known_devices(connection, answer, server);

    //Make sure a ble device is found
    if(local.get_path() != "") {
        std::shared_ptr<DBus::ObjectProxy> adapterObject = connection->create_object_proxy("org.bluez", local.get_path());

        //create listener for device added
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, BLEDeviceInterface)>> addSignal = 
            listen_for_device_added(connection, server);
        //Add reciever to listen for device removed signal
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> removeSignal = 
            listen_for_device_removed(connection, server); 


        //create buffer to recieve messages from site

        //listen for websocket events
        //rework listener so instead of just looping infinantly, use condition variable to wait for thread to return value
        //boolean that is meant to be written to and read from by different threads
        std::atomic_bool scan = false;
        std::atomic_bool attack = false;
        bool mainRun = true;
        
        while(mainRun) {
            int msgLen = server.get_command(buf);
            std::cout << "Printing json string:\n";
            for(int i = 0; i < msgLen; i++) {
                std::cout << buf[i] << "\n";
            }
            std::cout << "\n\n";

            JsonObject jsonCmd(buf, msgLen);
            int cmd = jsonCmd.get_item("command")->get_float();
            
            //Find a way to created detached threads
            ThreadPool pool(ATTACK_THREADS);

            //I think websocket is getting the command from the site incorrectly
            switch(cmd) {
                //if I have the get_command function wait until a command is recieved, I will need to have the bluetooth scanner as its own thread, ar async?
                case 1: { //start scanning
                    local.start_scan();
                    break;
                }
                case 2: { //stop scanning
                    local.stop_scan();
                    break;
                }
                case 3: { //start attack
                    local.stop_scan();
                    attack = true;
                    for(int i = 0; i < ATTACK_THREADS; i++) {
                        pool.enqueue([]{
                            for(int i = 0; i < 10; i++) {
                                std::cout << "Threaded loop test: " << i << "\n";
                            }
                        });
                    }
                    //start_attack(attack, jsonCmd, connection);
                    break;
                }
                case 4: { //end program execution
                    local.stop_scan();
                    mainRun = false;
                    std::cout << "Exiting program\n";
                    break;
                }
            }
            std::cout << "Main thread loop\n";
            sleep(.1);
        }
    }
    else {
        std::cout << "Error: Suitable Device not found\n";
        return 1;
    }
    return 0;
}
