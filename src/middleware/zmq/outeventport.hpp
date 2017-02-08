#ifndef ZMQ_OUTEVENTPORT_H
#define ZMQ_OUTEVENTPORT_H

#include "../../core/eventports/outeventport.hpp"
#include "../../core/modellogger.h"
#include <vector>
#include <iostream>
#include <string>
#include <mutex>
#include "zmqhelper.h"

namespace zmq{
     template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(Component* component, std::string name);
            void tx(T* message);

            void Startup(std::map<std::string, ::Attribute*> attributes);
            void Teardown();

            bool Activate();
            bool Passivate();
        private:
            std::mutex control_mutex_;

            bool configured_ = false;
            
            zmq::socket_t* socket_;
            std::vector<std::string> end_points_;
    }; 
};

template <class T, class S>
void zmq::OutEventPort<T, S>::tx(T* message){
    if(this->is_active() && socket_){
        std::string str = proto::encode(message);
        zmq::message_t data(str.c_str(), str.size());
        socket_->send(data);
        ::OutEventPort<T>::tx(message);
    }
};

template <class T, class S>
zmq::OutEventPort<T, S>::OutEventPort(Component* component, std::string name): ::OutEventPort<T>(component, name){
};


template <class T, class S>
void zmq::OutEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    end_points_.clear();

    if(attributes.count("publisher_address")){
        for(auto s : attributes["publisher_address"]->s){
            end_points_.push_back(s);
        }
        configured_ = true;
    }
};

template <class T, class S>
void zmq::OutEventPort<T, S>::Teardown(){
    Passivate();

    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    auto helper = ZmqHelper::get_zmq_helper();
    this->socket_ = helper->get_publisher_socket();
    if(!end_points_.empty()){
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
    return ::OutEventPort<T>::Activate();
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(socket_){
        delete socket_;
        socket_ = 0;
    }
    return ::OutEventPort<T>::Passivate();
};

#endif //ZMQ_INEVENTPORT_H
