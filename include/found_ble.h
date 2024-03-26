#include <parse_dbus.h>
#include <vector>
#include <map>


class FoundBLE : public BLEObject {
    std::vector<std::string> uuidList;
    std::vector<std::string> serviceTypes;
    std::string devPath;

    static const std::map<std::string, std::string> bleUUID;

    int get_service_name();

    public:
        //determine if object is ble or classic bluetooth
        int isBLE;
        FoundBLE(int a);

        int add_UUID(std::string uuid);
        std::vector<std::string> get_UUID();
        std::vector<std::string> get_services();
        int get_type();

        //void set_path(DBus::Path path);
        //std::string get_path();
};
