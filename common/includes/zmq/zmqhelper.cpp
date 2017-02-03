#include "zmqhelper.h"
#include <iostream>

zmq::ZmqHelper* zmq::ZmqHelper::singleton_ = 0;
std::mutex zmq::ZmqHelper::global_mutex_;

zmq::ZmqHelper* zmq::ZmqHelper::get_zmq_helper(){
    std::lock_guard<std::mutex> lock(global_mutex_);

    if(singleton_ == 0){
        singleton_ = new ZmqHelper();
        std::cout << "Constructed ZMQ Helper: " << singleton_ << std::endl;
    }
    return singleton_;
};

zmq::context_t* zmq::ZmqHelper::get_context(){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex);
    if(!context_){
        context_ = new zmq::context_t();
    }
    return context_;
};

zmq::socket_t* zmq::ZmqHelper::get_publisher_socket(){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex);
    zmq::socket_t* s = 0;
    if(context_){
        s = new zmq::socket_t(*context_, ZMQ_PUB);
    }
    return s;
};

zmq::socket_t* zmq::ZmqHelper::get_subscriber_socket(){
    //Acquire the Lock
    std::lock_guard<std::mutex> lock(mutex);
    zmq::socket_t* s = 0;
    if(context_){
        s = new zmq::socket_t(*context_, ZMQ_SUB);
        s->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    }
    return s;
};