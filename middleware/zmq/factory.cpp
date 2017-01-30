#include "factory.h"

#include <iostream>
#include <functional>

#include "message/tx.h"
#include "message/rx.h"

EventPort* zmq::construct_rx(std::string type, Component* component, std::string port_name){
    auto fn = component->get_callback(port_name);
    if(type == "Message" && fn){
        return zmq::Message::construct_rx(component, port_name, fn);
    }
    return 0;
}

EventPort* zmq::construct_tx(std::string type, Component* component, std::string port_name){
    if(type == "Message"){
        return zmq::Message::construct_tx(component, port_name);
    }
    return 0;
}
