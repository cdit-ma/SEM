#include <core/libportexports.h>

#include "convert.h"
#include "message_DCPS.hpp"

#include <ospl/ineventport.hpp>
#include <ospl/outeventport.hpp>

EventPort* construct_rx(std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        //Get the callback function
        auto fn = component->get_callback(port_name);    
        if(fn){
            p = new ospl::InEventPort<::Message, cdit::Message>(component, port_name, fn);
        }
    }
    return p;
};

EventPort* construct_tx(std::string port_name, Component* component){
    return new ospl::OutEventPort<::Message, cdit::Message>(component, port_name);
};