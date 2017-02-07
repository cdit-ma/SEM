#include <core/libportexports.h>

#include "../../proto/message/convert.h"

#include <zmq/ineventport.hpp>
#include <zmq/outeventport.hpp>

EventPort* ConstructRx(std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        //Get the callback function
        auto fn = component->GetCallback(port_name);    
        if(fn){
            p = new zmq::InEventPort<::Message, proto::Message>(component, port_name, fn);
        }
    }
    return p;
};

EventPort* ConstructTx(std::string port_name, Component* component){
    return new zmq::OutEventPort<::Message, proto::Message>(component, port_name);
};