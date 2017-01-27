#ifndef TEST_H
#define TEST_H

#include <functional>

#include <core/component.h>
#include <core/eventport.h>
#include <core/basemessage.h>

namespace zmq{
    EventPort* construct_rx(std::string type, Component* component, std::string port_name, std::function<void (::BaseMessage*)> callback_function);
    EventPort* construct_tx(std::string type, Component* component, std::string port_name);
};


#endif //TEST_H