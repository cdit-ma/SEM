#include "zmqtxmessage.h"


//Includes the ::Message and proto::Message
#include "../proto/message/messageconvert.h"

//Include the templated OutEventPort Implementation for ZMQ
#include "zmq/outeventport.hpp"

EXPORT_FUNC ::OutEventPort<::Message>* zmq::construct_TxMessage(Component* component, std::string endpoint){
    //Construct a vector of the end_points this port should connect to.
    std::vector<std::string> v;
    v.push_back(endpoint);

    auto p = new zmq::OutEventPort<::Message, proto::Message>(component, v);
    return p;
}