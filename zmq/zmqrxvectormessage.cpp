#include "zmqrxvectormessage.h"

//Includes the ::Message and proto::Message
#include "../proto/vectormessage/vectormessageconvert.h"

//Include the templated OutEventPort Implementation for ZMQ
#include "zmq/ineventport.hpp"

::InEventPort<::VectorMessage>* zmq::construct_RxVectorMessage(Component* component, std::function<void (::VectorMessage*)> callback_function, std::string endpoint){
    //Construct a vector of the end_points this port should connect to.
    std::vector<std::string> v;
    v.push_back(endpoint);

    auto p = new zmq::InEventPort<::VectorMessage, cdit::VectorMessage>(component, callback_function, v);
    return p;
}