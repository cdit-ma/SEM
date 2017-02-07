#include <core/libportexports.h>

#include "tx.h"
#include "rx.h"

EventPort* ConstructRx(std::string port_name, Component* component){
    EventPort* p = 0;
    if(component){
        //Get the callback function
        auto fn = component->GetCallback(port_name);    
        if(fn){
            p = rti::Message::ConstructRx(component, port_name, fn);
        }
    }
    return p;
};

EventPort* ConstructTx(std::string port_name, Component* component){
    return rti::Message::ConstructTx(component, port_name);
};

void DestructEventport(EventPort* port){
    if(port){
        delete port;
    }
};