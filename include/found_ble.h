#include <parse_dbus.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>


class FoundBLE : public BLEObject {
    //Device can advertise multple uuid
    std::vector<std::string> uuidList;
    std::vector<std::string> serviceTypes;
    bool music = false;

    std::map <std::string, std::vector<std::string>> deviceAttributes;

    static const std::map<std::string, std::string> bleUUID;

    int get_service_name();
    void copy_value(std::string str, char arr[], int arrSize, int &pos);
    void add_digit(char dig, char arr[], int arrSize, int &pos);

    public:
        //determine if object is ble or classic bluetooth
        int isBLE;
        FoundBLE(int a);

        void add_value(std::string key, std::string value);
        void add_vect(std::string key, std::vector<std::string> vect);

        int obj_json(char jsonArr[], int arrLen);
        //void set_path(DBus::Path path);
        //std::string get_path();
        //add function to pull value from map
};
