#ifndef ZMQ_LIBEXPORT_H
#define ZMQ_LIBEXPORT_H

#include "factory.h"
extern "C" EventPort* construct_rx(std::string port, Component* component, std::string name)
{
    return zmq::construct_rx(port, component, name);
};

extern "C" EventPort* construct_tx(std::string port, Component* component, std::string name)
{
    return zmq::construct_tx(port, component, name);
};

extern "C" std::string get_middleware(){
    return "zmp";
}

#endif //ZMQ_LIBEXPORT_H