#ifndef CORE_LIBEXPORTS_H
#define CORE_LIBEXPORTS_H

#include "eventport.h"
#include "component.h"
#include <string>

extern "C"{
    EventPort* construct_rx(std::string port_type, std::string port_name, Component* component);
    EventPort* construct_tx(std::string port_type, std::string port_name, Component* component);
    void destruct_eventport(EventPort* port);
}

#endif //CORE_LIBEXPORTS_H