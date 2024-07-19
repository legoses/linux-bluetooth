#include <dbus-cxx.h>
#include <string>
#include <local_adapter.h>
#include <found_ble.h>
#include <websocket.h>
#include <json.h>

typedef std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> BLEDeviceObject;
typedef std::map<std::string, std::map<std::string, DBus::Variant>> BLEDeviceInterface;
const int ATTACK_THREADS = 4;


LocalAdapter parse_known_devices(std::shared_ptr<DBus::Connection> connection, BLEDeviceObject &devices, std::vector<std::string> &paths, std::mutex &mtx, Web::WebsocketServer &server) {
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
        paths.push_back(pth);

        while(itr != itrEnd) {
            if(itr->first == adapterName) {
                pth = (std::string) it->first;
                paths.push_back(pth);
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


void update_site(std::mutex &mtx, Web::WebsocketServer &server) {

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
int prepare_json(char arr[], const int size, BLEDeviceInterface &device, bool operation) {
    char *address = device["Address"].to_string().c_str();
    char *name = device["Name"].to_string().c_str();
    int addrSize = sizeof(address) / sizeof(char);
    int nameSize = sizeof(name) / sizeof(char);
    int pos = 0;

    arr[0] = '{';
    arr[1] = '"';

    memcpy(&arr[2], "Address", 7);
    pos = 10;
    arr[pos] = ':';
    memcpy(&arr[pos], device["Address"], addrSize);
    pos++;
    arr[pos] = '"';
    pos++;
    arr[pos] = ':';
    pos++;
    arr[pos] = '"';
    pos++;
    memcpy(&arr[pos], "Name", 4);
    pos+=4;
    memcpy(&arr[pos], device["Name"], nameSize);
    pos++;
    pos+=nameSize;
    arr[pos] = '"';
    std::cout << "arr test: " << arr << "\n";
    return pos;
}


void device_add(DBus::Path path, BLEDeviceInterface device, std::mutex &mtx, Web::WebsocketServer &server) {
    std::cout << "Device added at path: " << device["Address"] << "\n";
    const int bufSize = 200
    char data[bufSize];
    int size = prepare_json(data, bufSize, device, true);

    //server.send_data(tst, sizeof(tst)/sizeof(char), false);
}


void device_remove(DBus::Path path, std::vector<std::string> devices, std::mutex &mtx, Web::WebsocketServer &server) {
    std::cout << "Device removed\n";
}


std::shared_ptr<DBus::SignalProxy<void(DBus::Path, BLEDeviceInterface)>> listen_for_device_added(std::shared_ptr<DBus::Connection> &connection, std::mutex &mtx, Web::WebsocketServer &server) {
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
    signal->connect(sigc::bind(sigc::ptr_fun(&device_add), std::ref(mtx), std::ref(server)));

    std::cout << "Running\n" << std::flush;
    return signal;
}


std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> listen_for_device_removed(
            std::shared_ptr<DBus::Connection> &connection,
            std::mutex &mtx,
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
    signal->connect(sigc::bind(sigc::ptr_fun(&device_remove), std::ref(mtx), std::ref(server)));

    std::cout << "Running\n" << std::flush;
    return signal;
}


int main() {
    //allocate memory for pointer vector
    std::vector<std::string> paths;
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
    LocalAdapter local = parse_known_devices(connection, answer, paths, mtx, server);

    //Make sure a ble device is found
    if(local.get_path() != "") {
        std::shared_ptr<DBus::ObjectProxy> adapterObject = connection->create_object_proxy("org.bluez", local.get_path());

        //create listener for device added
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, BLEDeviceInterface)>> addSignal = 
            listen_for_device_added(connection, mtx, server);
        //Add reciever to listen for device removed signal
        std::shared_ptr<DBus::SignalProxy<void(DBus::Path, std::vector<std::string>)>> removeSignal = 
            listen_for_device_removed(connection, mtx, server); 


        //create buffer to recieve messages from site

        //listen for websocket events
        //rework listener so instead of just looping infinantly, use condition variable to wait for thread to return value
        //boolean that is meant to be written to and read from by different threads
        std::atomic_bool run = false;
        std::atomic_bool attack = false;
        bool mainRun = true;
        if(local.get_path() != "") {
            std::cout << "Scanning\n";
            local.start_scan();
            while(true) {
                sleep(1);
            }
            //sleep(5);
            //std::cout << "Stopping scan\n";
            //local.stop_scan();
        }
    }
    return 0;
}
