#include "zmqrxmessage.h"

//Includes the ::Message and proto::Message
#include "../proto/convert.h"

//Include the templated InEventPort Implementation for ZMQ
#include <zmq/ineventport.hpp>

EXPORT_FUNC ::InEventPort<::Message>* zmq::construct_RxMessage(Component* component, std::string name, std::function<void (::BaseMessage*)> callback_function){
    std::function<void (::Message*)> test = static_cast<std::function<void (::Message*) > >(callback_function);

    auto p = new zmq::InEventPort<::Message, proto::Message>(component, name, test);
    return p;
}