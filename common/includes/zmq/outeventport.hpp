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
            OutEventPort(Component* component, std::string name);
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
zmq::OutEventPort<T, S>::OutEventPort(Component* component, std::string name): ::OutEventPort<T>(component, name){
};


template <class T, class S>
void zmq::OutEventPort<T, S>::startup(std::map<std::string, ::Attribute*> attributes){
    end_points_.clear();

    if(attributes.count("publisher_address")){
        for(auto s : attributes["publisher_address"]->s){
            end_points_.push_back(s);
        }
    }

    if(!end_points_.empty()){
        auto helper = ZmqHelper::get_zmq_helper();
        this->socket_ = helper->get_publisher_socket();
    
        
        for(auto e: end_points_){
            std::cout << "ZMQ::OutEventPort::" << this->get_name() <<  " Bind: " << e << std::endl;
            try{
                //Bind the addresses provided
                this->socket_->bind(e.c_str());   
            }catch(zmq::error_t){
                std::cout << "Couldn't Bind!" << std::endl;
            }
        }
    }
};

template <class T, class S>
void zmq::OutEventPort<T, S>::teardown(){
};

#endif //ZMQ_INEVENTPORT_H
