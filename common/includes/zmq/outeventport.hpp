#ifndef ZMQ_OUTEVENTPORT_H
#define ZMQ_OUTEVENTPORT_H

#include "../core/eventports/outeventport.hpp"
#include <vector>
#include <iostream>
#include <string>
#include "helper.hpp"

namespace zmq{
     template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(Component* component, std::vector<std::string> end_points);
            void tx(T* message);

            void startup(std::map<std::string, ::Attribute*> attributes);
            void teardown();
        private:
            zmq::socket_t* socket_;
            std::vector<std::string> end_points_;
    }; 
};

template <class T, class S>
void zmq::OutEventPort<T, S>::tx(T* message){
    if(this->is_active()){
        std::string str = proto::encode(message);
        if(socket_){
            zmq::message_t data(str.c_str(), str.size());
            socket_->send(data);
        }
    }
};

template <class T, class S>
zmq::OutEventPort<T, S>::OutEventPort(Component* component, std::vector<std::string> end_points): ::OutEventPort<T>(component){
    /*this->end_points_ = end_points;

    auto helper = ZmqHelper::get_zmq_helper();
    this->socket_ = helper->get_publisher_socket();
    
    for(auto end_point: end_points_){
        std::cout << "Binding To: " << end_point << std::endl;
        //Connect to the publisher
        this->socket_->bind(end_point.c_str());   
    }*/
};

template <class T, class S>
void zmq::OutEventPort<T, S>::startup(std::map<std::string, ::Attribute*> attributes){
    
    this->end_points_.clear();
    if(attributes.count("sender_addresses")){
        auto attr = attributes["sender_addresses"];
        if(attr->type == AT_STRINGLIST){
            for(auto s : attr->s){
                std::cout << "ZMQ:INEVENTPORT Got: " << s << std::endl;
                end_points_.push_back(s);
            }
        }   
    }

    if(!end_points_.empty()){
        auto helper = ZmqHelper::get_zmq_helper();
        this->socket_ = helper->get_publisher_socket();
    
        for(auto end_point: end_points_){
            std::cout << "Binding To: " << end_point << std::endl;
            //Connect to the publisher
            this->socket_->bind(end_point.c_str());   
        }
    }else{
        std::cout << "NO RECIEVERS!" << std::endl;
    }
};

template <class T, class S>
void zmq::OutEventPort<T, S>::teardown(){
};

#endif //ZMQ_INEVENTPORT_H
