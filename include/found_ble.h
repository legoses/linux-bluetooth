#include <parse_dbus.h>
#include <vector>
#include <map>


class FoundBLE : public BLEObject {
    std::vector<std::string> uuidList;
    std::vector<std::string> serviceTypes;
    //determine if object is ble or classic bluetooth

    static const std::map<std::string, std::string> bleUUID;

    int get_service_name();

    public:
        const int isBLE;
        FoundBLE(int a);

        int add_UUID(std::string uuid);
        std::vector<std::string> get_UUID();
        std::vector<std::string> get_services();
        int get_type();
};
