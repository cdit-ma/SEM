#include "zmqtxmessage.h"


//Includes the ::Message and proto::Message
#include "../proto/message/messageconvert.h"

//Include the templated OutEventPort Implementation for ZMQ
#include "zmq/outeventport.hpp"

EXPORT_FUNC ::OutEventPort<::Message>* zmq::construct_TxMessage(Component* component, std::string name){
    auto p = new zmq::OutEventPort<::Message, proto::Message>(component, name);
    return p;
}