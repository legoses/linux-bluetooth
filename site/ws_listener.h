#ifndef WS_LISTENER_H
#define WS_LISTENER_H

#include <stdint.h>
#include <cstring>

class WSListener {
    //function pointer
    void (*func_cb)(uint8_t[], int);
    int maxPktSize = 2000;
public:
    WSListener(void (*funcptr)(uint8_t[], int));
    void listener();
    void threaded_listener();
};
#endif
