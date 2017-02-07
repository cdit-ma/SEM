#include <core/libportexports.h>

#include "convert.h"
#include "message.hpp"

#include <rti/ineventport.hpp>
#include <rti/outeventport.hpp>

EventPort* ConstructRx(std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        //Get the callback function
        auto fn = component->GetCallback(port_name);    
        if(fn){
            p = new rti::InEventPort<::Message, cdit::Message>(component, port_name, fn);
        }
    }
    return p;
};

EventPort* ConstructTx(std::string port_name, Component* component){
    return new rti::OutEventPort<::Message, cdit::Message>(component, port_name);
};