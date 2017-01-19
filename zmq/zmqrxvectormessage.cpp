#include "zmqrxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated OutEventPort Implementation for ZMQ
#include "zmq/ineventport.hpp"

EXPORT_FUNC ::InEventPort<::VectorMessage>* zmq::construct_RxVectorMessage(Component* component, std::string name, std::function<void (::VectorMessage*)> callback_function){
    auto p = new zmq::InEventPort<::VectorMessage, cdit::VectorMessage>(component, name, callback_function);
    return p;
}