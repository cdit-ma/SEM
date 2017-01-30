#ifndef TEST_H
#define TEST_H

#include <core/component.h>
#include <core/eventport.h>
#include <core/basemessage.h>

namespace zmq{
    EventPort* construct_rx(std::string type, Component* component, std::string port_name);
    EventPort* construct_tx(std::string type, Component* component, std::string port_name);
};

extern "C" EventPort* construct_rx(std::string port, Component* component, std::string name)
{
    return zmq::construct_rx(port, component, name);
};

extern "C" EventPort* construct_tx(std::string port, Component* component, std::string name)
{
    return zmq::construct_tx(port, component, name);
};



#endif //TEST_H