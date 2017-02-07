#include <core/libportexports.h>

#include "convert.h"
#include "message_DCPS.hpp"

#include <opensplice/ineventport.hpp>
#include <opensplice/outeventport.hpp>

EventPort* ConstructRx(std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        //Get the callback function
        auto fn = component->GetCallback(port_name);    
        if(fn){
            p = new ospl::InEventPort<::Message, cdit::Message>(component, port_name, fn);
        }
    }
    return p;
};

EventPort* ConstructTx(std::string port_name, Component* component){
    return new ospl::OutEventPort<::Message, cdit::Message>(component, port_name);
};