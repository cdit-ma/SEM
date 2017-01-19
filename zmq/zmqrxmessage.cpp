#include "zmqrxmessage.h"

//Includes the ::Message and proto::Message
#include "../proto/message/messageconvert.h"

//Include the templated InEventPort Implementation for ZMQ
#include "zmq/ineventport.hpp"

EXPORT_FUNC ::InEventPort<::Message>* zmq::construct_RxMessage(Component* component, std::string name, std::function<void (::Message*)> callback_function){
    auto p = new zmq::InEventPort<::Message, proto::Message>(component, name, callback_function);
    return p;
}