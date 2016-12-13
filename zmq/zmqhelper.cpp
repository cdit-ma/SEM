#include "zmqhelper.h"

zmq::ZmqHelper* zmq::ZmqHelper::singleton_ = 0;


zmq::ZmqHelper* zmq::ZmqHelper::get_zmq_helper(){
    if(singleton_ == 0){
        singleton_ = new ZmqHelper();
    }
    return singleton_;
}

zmq::context_t* zmq::ZmqHelper::get_context(){
    if(context_ == 0){
        context_ = new zmq::context_t(1); 
    }
    return context_;
}

zmq::socket_t* zmq::ZmqHelper::get_subscriber_socket(){
    auto socket = new zmq::socket_t(*get_context(), ZMQ_SUB);
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    return socket;
}

zmq::socket_t* zmq::ZmqHelper::get_publisher_socket(){
    auto socket = new zmq::socket_t(*get_context(), ZMQ_PUB);
    return socket;
}