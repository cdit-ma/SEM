#include <core/libportexports.h>

#include "message/tx.h"
#include "message/rx.h"

EventPort* construct_rx(std::string port_type, std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        //Get the callback function
        auto fn = component->get_callback(port_name);    
        if(fn){
            if(port_type == "Message"){
                p = zmq::Message::construct_rx(component, port_name, fn);
            }
        }
    }
    return p;
}

EventPort* construct_tx(std::string port_type, std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        if(port_type == "Message"){
            p = zmq::Message::construct_tx(component, port_name);
        }
    }
    return p;
};

void destruct_eventport(EventPort* port){
    if(port){
        delete port;
    }
};