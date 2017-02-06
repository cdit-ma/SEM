#ifndef CORE_LIBPORTEXPORTS_H
#define CORE_LIBPORTEXPORTS_H

#include "eventport.h"
#include "component.h"
#include <string>

extern "C"{
    EventPort* construct_rx(std::string port_name, Component* component);
    EventPort* construct_tx(std::string port_name, Component* component);
    void destruct_eventport(EventPort* port){
        if(port){
            delete port;
        }
    }
}

#endif //CORE_LIBPORTEXPORTS_H