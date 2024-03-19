#include <found_ble.h>

FoundBLE::FoundBLE(int a)
    : isBLE(a)
{

}

//only add a uuid if this is a ble device. Otherwise return 1
int FoundBLE::add_UUID(std::string uuid) {
    if(isBLE == 1) {
        this->uuidList.push_back(uuid);
        return 0;
    }
    else {
        return 1;
    }
}
