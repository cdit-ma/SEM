#ifndef OSPL_LIBEXPORT_H
#define OSPL_LIBEXPORT_H

#include "factory.h"
extern "C" EventPort* construct_rx(std::string port, Component* component, std::string name)
{
    return ospl::construct_rx(port, component, name);
};

extern "C" EventPort* construct_tx(std::string port, Component* component, std::string name)
{
    return ospl::construct_tx(port, component, name);
};

extern "C" std::string get_middleware(){
    return "ospl";
}

#endif //OSPL_LIBEXPORT_H