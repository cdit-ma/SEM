#include <core/libportexports.h>

#include "tx.h"
#include "rx.h"

EventPort* ConstructRx(std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        //Get the callback function
        auto fn = component->GetCallback(port_name);    
        if(fn){
            p = zmq::Message::ConstructRx(component, port_name, fn);
        }
    }
    return p;
};

void DestructEventport(EventPort* port){
    if(port){
        delete port;
    }
};

EventPort* ConstructTx(std::string port_name, Component* component){
    return zmq::Message::ConstructTx(component, port_name);
};