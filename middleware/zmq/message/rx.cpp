#include "rx.h"

#include <zmq/ineventport.hpp>

::InEventPort<::Message>* zmq::Message::construct_rx(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    return new zmq::InEventPort<::Message, proto::Message>(component, name, callback_function);
}

EventPort* construct_rx(std::string port_name, Component* component){
    EventPort* p = 0;
    //Get the callback function
    auto fn = component->get_callback(port_name);    
    if(fn){
        p = zmq::Message::construct_rx(component, port_name, fn);
    }
    return p;
}

void destruct_eventport(EventPort* port){
    if(port){
        delete port;
    }
};
