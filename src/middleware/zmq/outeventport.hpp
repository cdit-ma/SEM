#ifndef ZMQ_OUTEVENTPORT_H
#define ZMQ_OUTEVENTPORT_H

#include "../../core/eventports/outeventport.hpp"
#include "../../core/eventport.h"
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
            bool Teardown();

            bool Activate();
            bool Passivate();

        private:
            void zmq_sleep();


            std::mutex control_mutex_;
            std::mutex ready_mutex_;
            bool ready_ = false;
            bool configured_ = false;
            
            zmq::socket_t* socket_;
            std::vector<std::string> end_points_;
    }; 
};

template <class T, class S>
void zmq::OutEventPort<T, S>::tx(T* message){
    std::lock_guard<std::mutex> lock(ready_mutex_);
    if(ready_ && this->is_active() && socket_){
        std::string str = proto::encode(message);
        zmq::message_t data(str.c_str(), str.size());
        socket_->send(data);
        ::OutEventPort<T>::tx(message);
    }
};

template <class T, class S>
zmq::OutEventPort<T, S>::OutEventPort(Component* component, std::string name): ::OutEventPort<T>(component, name, "zmq"){
};


template <class T, class S>
void zmq::OutEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    end_points_.clear();

    if(attributes.count("publisher_address")){
        for(auto s : attributes["publisher_address"]->StringList()){
            end_points_.push_back(s);
        }
        configured_ = true;
    }
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::Teardown(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::Teardown()){
        configured_ = false;
        return true;
    }
    return false;
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(::OutEventPort<T>::Activate()){
        auto helper = ZmqHelper::get_zmq_helper();
        this->socket_ = helper->get_publisher_socket();
        for(auto e: end_points_){
            try{
                std::cout << "ZMQ::OutEventPort::" << this->get_name() <<  " Bind: " << e << std::endl;
                //Bind the addresses provided
                this->socket_->bind(e.c_str());
            }catch(zmq::error_t){
                std::cout << "Couldn't Bind!" << std::endl;
            }
        }
        //Do some threadage
        auto t = std::thread(&zmq::OutEventPort<T, S>::zmq_sleep, this);
        t.detach();

        //std::this_thread::sleep_for(std::chrono::milliseconds());
        EventPort::LogActivation();
        return true;
    }
    return false;
};

template <class T, class S>
bool zmq::OutEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(::OutEventPort<T>::Passivate()){
        if(socket_){
            delete socket_;
            socket_ = 0;
            EventPort::LogPassivation();
        }
        return true;
    }
    return false;
};


template <class T, class S>
void zmq::OutEventPort<T, S>::zmq_sleep(){
    std::lock_guard<std::mutex> lock(ready_mutex_);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    ready_ = true;
};

#endif //ZMQ_INEVENTPORT_H
