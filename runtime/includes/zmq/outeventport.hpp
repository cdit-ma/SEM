#ifndef ZMQ_OUTEVENTPORT_H
#define ZMQ_OUTEVENTPORT_H

#include "../globalinterfaces.h"
#include <vector>
#include <iostream>
#include <string>
#include "helper.hpp"

namespace zmq{
     template <class T, class S> class Zmq_OutEventPort: public ::OutEventPort<T>{
        public:
            Zmq_OutEventPort(::OutEventPort<T>* port, std::vector<std::string> end_points);
            void tx_(T* message);
        private:

            zmq::socket_t* socket_;
            ::OutEventPort<T>* port_;
            std::vector<std::string> end_points_;
    }; 
};

template <class T, class S>
void zmq::Zmq_OutEventPort<T, S>::tx_(T* message){
    std::string str = proto::encode(message);
    if(socket_){
        zmq::message_t data(str.c_str(), str.size());
        socket_->send(data);
    }
};

template <class T, class S>
zmq::Zmq_OutEventPort<T, S>::Zmq_OutEventPort(::OutEventPort<T>* port, std::vector<std::string> end_points){
    this->port_ = port;
    this->end_points_ = end_points;

    auto helper = ZmqHelper::get_zmq_helper();
    this->socket_ = helper->get_publisher_socket();
    
    for(auto end_point: end_points_){
        std::cout << "Binding To: " << end_point << std::endl;
        //Connect to the publisher
        this->socket_->bind(end_point.c_str());   
    }
};

#endif //ZMQ_INEVENTPORT_H
