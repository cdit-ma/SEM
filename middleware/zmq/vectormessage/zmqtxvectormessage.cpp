#include "zmqtxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated OutEventPort Implementation for ZMQ
#include "zmq/outeventport.hpp"

EXPORT_FUNC ::OutEventPort<::VectorMessage>* zmq::construct_TxVectorMessage(Component* component, std::string name){
    //Construct a vector of the end_points this port should connect to.
    auto p = new zmq::OutEventPort<::VectorMessage, cdit::VectorMessage>(component, name);
    return p;
}