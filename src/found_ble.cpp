#include <found_ble.h>

FoundBLE::FoundBLE(int a)
    : isBLE(a)
{}


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


//only add a uuid if this is a ble device. Otherwise return 1
void FoundBLE::add_value(std::string key, std::string value) {
    if(this->deviceAttributes.count(key) > 0) {
        this->deviceAttributes[key].push_back(value);
    }
    else {
        std::vector<std::string> str;
        str.push_back(value);
        this->deviceAttributes[key] = str;
    }
}


void FoundBLE::add_vect(std::string key, std::vector<std::string> vect) {
    if(this->deviceAttributes.count(key) > 0) {
        //copy contents of vect to deviceAttributes map
        //back_inserter create push_back function, allowing items to be placed in back of vector
        std::move(
            vect.begin(), 
            vect.end(), 
            std::back_inserter(this->deviceAttributes[key]));
    }
    else {
        this->deviceAttributes[key] = vect;
    }
}

//add digit to json arr
void FoundBLE::add_digit(char dig, char arr[], int arrSize, int &pos) {
    if(pos+1 < arrSize) {
        arr[pos] = dig;
        pos++;
    }
}


void FoundBLE::copy_value(std::string str, char arr[], int arrSize, int &pos)  {
    //make sure value will not overflow array
    if(pos + str.size()+2 < arrSize) {
        add_digit('"', arr, arrSize, pos);
        for(int i = 0; i < str.size(); i++) {
           arr[pos+i] = str[i]; 
        }
        pos+=str.size();
        add_digit('"', arr, arrSize, pos);
    }
}

//convert map to json format
int FoundBLE::obj_json(char jsonArr[], int arrLen) {
    std::map<std::string, std::vector<std::string>>::iterator it = deviceAttributes.begin();

    int arrPos = 0;
    add_digit('{', jsonArr, arrLen, arrPos);
    while(it != deviceAttributes.end()) {
        std::cout << "Entering while loop\n";
        //if there is only one value is vect, do not creat an array
        if(it->second.size() == 1) {
            std::cout << "not vector\n";
            //copy value to arr and surround by quotes
            copy_value(it->first, jsonArr, arrLen, arrPos);
            add_digit(':', jsonArr, arrLen, arrPos);
            
            copy_value(it->second[0], jsonArr, arrLen, arrPos);
            add_digit(',', jsonArr, arrLen, arrPos);
        }
        //if the vect contains multiple vlaues, create an array
        else if(it->second.size() > 1) {
            std::cout << "vector\n";
            copy_value(it->first, jsonArr, arrLen, arrPos); 
            add_digit(':', jsonArr, arrLen, arrPos);
            add_digit('[', jsonArr, arrLen, arrPos);
            
            for(std::string &val : it->second) {
                copy_value(val, jsonArr, arrLen, arrPos); 

                if(val != it->second[-1]) {
                    add_digit(',', jsonArr, arrLen, arrPos);
                }
            }

            add_digit(']', jsonArr, arrLen, arrPos);
            add_digit(',', jsonArr, arrLen, arrPos);
        }
        ++it;
    }
    add_digit('}', jsonArr, arrLen, arrPos);
    std::cout << "done\n";

    //return size of arrai
    return arrPos+1;
}
