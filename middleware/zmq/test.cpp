#include "test.h"

#include "message/tx.h"
#include "message/rx.h"

EXPORT_FUNC EventPort* zmq::construct_rx(std::string type, Component* component, std::string port_name, std::function<void (::BaseMessage*)> callback_function){
    if(type == "Message"){
        return zmq::Message::construct_rx(component, port_name, callback_function);
    }
    return 0;
}

EXPORT_FUNC EventPort* zmq::construct_tx(std::string type, Component* component, std::string port_name){
    if(type == "Message"){
        return zmq::Message::construct_tx(component, port_name);
    }

    return 0;
}
