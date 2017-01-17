#include "zmqrxmessage.h"

//Includes the ::Message and proto::Message
#include "../proto/message/messageconvert.h"

//Include the templated InEventPort Implementation for ZMQ
#include "zmq/ineventport.hpp"

::InEventPort<::Message>* zmq::construct_RxMessage(Component* component, std::function<void (::Message*)> callback_function, std::string endpoint){
    //Construct a vector of the end_points this port should connect to.
    std::vector<std::string> v;
    v.push_back(endpoint);

    auto p = new zmq::InEventPort<::Message, proto::Message>(component, callback_function, v);
    return p;
}