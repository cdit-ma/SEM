#ifndef RTI_LIBEXPORT_H
#define RTI_LIBEXPORT_H

#include "factory.h"
extern "C" EventPort* construct_rx(std::string port, Component* component, std::string name)
{
    return rti::construct_rx(port, component, name);
};

extern "C" EventPort* construct_tx(std::string port, Component* component, std::string name)
{
    return rti::construct_tx(port, component, name);
};

extern "C" std::string get_middleware(){
    return "rti";
}

#endif //rti_LIBEXPORT_H