#include <parse_dbus.h>
#include <vector>
#include <map>


class FoundBLE : public BLEObject {
    std::vector<std::string> uuidList;
    std::vector<std::string> serviceTypes;
    //determine if object is ble or classic bluetooth

    static const std::map<std::string, std::string> bleUUID = {
        {"0000180a-0000-1000-8000-00805f9b34fb", "device information service"},
        {"e95d93b0-251d-470a-a062-fa1922dfa9a8", "dfu control service"},
        {"e95d93af-251d-470a-a062-fa1922dfa9a8", "event service"},
        {"e95d9882-251d-470a-a062-fa1922dfa9a8", "button service"},
        {"e95d6100-251d-470a-a062-fa1922dfa9a8", "temperature service"},
        {"e95dd91d-251d-470a-a062-fa1922dfa9a8", "led service"},
        {"00002a05-0000-1000-8000-00805f9b34fb", "service changed"},
        {"e95d93b1-251d-470a-a062-fa1922dfa9a8", "dfu control"},
        {"00002a05-0000-1000-8000-00805f9b34fb", "service changed"},
        {"00002a24-0000-1000-8000-00805f9b34fb", "model number string"},
        {"00002a25-0000-1000-8000-00805f9b34fb", "serial number string"},
        {"00002a26-0000-1000-8000-00805f9b34fb", "firmware revision string"},
        {"e95d9775-251d-470a-a062-fa1922dfa9a8", "micro:bit event"},
        {"e95d5404-251d-470a-a062-fa1922dfa9a8", "client event"},
        {"e95d23c4-251d-470a-a062-fa1922dfa9a8", "client requirements"},
        {"e95db84c-251d-470a-a062-fa1922dfa9a8", "micro:bit requirements"},
        {"e95dda90-251d-470a-a062-fa1922dfa9a8", "button a state"},
        {"e95dda91-251d-470a-a062-fa1922dfa9a8", "button b state"},
        {"e95d9250-251d-470a-a062-fa1922dfa9a8", "temperature"},
        {"e95d93ee-251d-470a-a062-fa1922dfa9a8", "led text"},
        {"00002902-0000-1000-8000-00805f9b34fb", "client characteristic configuration"},
    };

    int get_service_name();

    public:
        const int isBLE;
        FoundBLE(int a);

        int add_UUID(std::string uuid);
        std::vector<std::string> get_UUID();
        std::vector<std::string> get_services();
};