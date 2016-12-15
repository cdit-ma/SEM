#ifndef ZMQHELPER_H
#define ZMQHELPER_H

#include <string>
#include "zmq.hpp"

namespace zmq{
    class ZmqHelper{
        public:
            ZmqHelper();
            static ZmqHelper* get_zmq_helper();
            zmq::context_t* get_context();
            zmq::socket_t* get_publisher_socket();
            zmq::socket_t* get_subscriber_socket();
        private:
            zmq::context_t* context_;
    };
};

inline zmq::ZmqHelper* zmq::ZmqHelper::get_zmq_helper(){
    static zmq::ZmqHelper* singleton_ = 0;
                
    if(singleton_ == 0){
        singleton_ = new ZmqHelper();
    }
    
    return singleton_;
};

inline zmq::ZmqHelper::ZmqHelper(){
    this->context_ = new zmq::context_t(10);
};

inline zmq::context_t* zmq::ZmqHelper::get_context(){
    return context_;
};

inline zmq::socket_t* zmq::ZmqHelper::get_publisher_socket(){
    auto socket = new zmq::socket_t(*get_context(), ZMQ_PUB);
    return socket;
};
inline zmq::socket_t* zmq::ZmqHelper::get_subscriber_socket(){
    auto socket = new zmq::socket_t(*get_context(), ZMQ_SUB);
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    return socket;
};
#endif //ZMQHELPER_H

