#include <core/libportexports.h>

#include "../../proto/message/convert.h"

#include <zmq/ineventport.hpp>
#include <zmq/outeventport.hpp>

EventPort* construct_rx(std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        //Get the callback function
        auto fn = component->get_callback(port_name);    
        if(fn){
            p = new zmq::InEventPort<::Message, proto::Message>(component, port_name, fn);
        }
    }
    return p;
};


EventPort* construct_tx(std::string port_name, Component* component){
    return new zmq::OutEventPort<::Message, proto::Message>(component, port_name);
};