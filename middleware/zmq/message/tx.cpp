#include "tx.h"
#include <zmq/outeventport.hpp>

EXPORT_FUNC ::OutEventPort<::Message>* zmq::Message::construct_tx(Component* component, std::string name){
    return new zmq::OutEventPort<::Message, proto::Message>(component, name);
}