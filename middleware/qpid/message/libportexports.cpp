#include <core/libportexports.h>

#include "../../proto/message/convert.h"

#include <qpid/ineventport.hpp>
#include <qpid/outeventport.hpp>

EventPort* construct_rx(std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        //Get the callback function
        auto fn = component->get_callback(port_name);    
        if(fn){
            p = new qpid::InEventPort<::Message, proto::Message>(component, port_name, fn);
        }
    }
    return p;
};


EventPort* construct_tx(std::string port_name, Component* component){
    return new qpid::OutEventPort<::Message, proto::Message>(component, port_name);
};