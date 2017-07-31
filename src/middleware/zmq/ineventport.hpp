#ifndef ZMQ_INEVENTPORT_H
#define ZMQ_INEVENTPORT_H

#include "../../core/eventports/ineventport.hpp"
#include "../../core/eventport.h"
#include <vector>
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include "zmqhelper.h"

namespace zmq{
     template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function);
            ~InEventPort();

            void Startup(std::map<std::string, ::Attribute*> attributes);

            bool Teardown();
            bool Activate();
            bool Passivate();

        private:
            void zmq_loop();

            std::thread* zmq_thread_ = 0;
            std::string terminate_endpoint_;
            std::vector<std::string> end_points_;

            std::mutex control_mutex_;
            std::mutex ready_mutex_;
            std::condition_variable ready_lock_condition_;
            bool configured_ = false;
    }; 
};

template <class T, class S>
void zmq::InEventPort<T, S>::zmq_loop(){
    auto helper = ZmqHelper::get_zmq_helper();
    auto socket = helper->get_subscriber_socket();

    try{
        socket->connect(terminate_endpoint_.c_str());    
        for(auto end_point: end_points_){
            std::cout << "zmq::InEventPort<T, S>::zmq_loop(): " << this->get_name() << " Connecting To: " << end_point << std::endl;
            //Connect to the publisher
            socket->connect(end_point.c_str());   
        }
    }catch(zmq::error_t ex){
        std::cerr << "zmq::InEventPort<T, S>::zmq_loop(): Couldn't connect to endpoints!" << std::endl;
    }

    {
        //Notify activation that we are done!
        std::unique_lock<std::mutex> lock(ready_mutex_);
        ready_lock_condition_.notify_all();
    }

    //Construct a new ZMQ Message to store the resulting message in.
    zmq::message_t *data = new zmq::message_t();

    while(true){
		try{
            //Wait for next message
            socket->recv(data);
            
            //If we have a valid message
            if(data->size() > 0){
                std::string msg_str(static_cast<char *>(data->data()), data->size());
                auto m = proto::decode<S>(msg_str);
                this->EnqueueMessage(m);
            }else{
                //std::cout << "Got Termination Message!" << std::endl;
                //Got Terminate message, length 0
                break;
            }
        }catch(zmq::error_t ex){
            //Do nothing with an error.
            std::cerr << "zmq::InEventPort<T, S>::zmq_loop(): ZMQ_Error: " << ex.num() << std::endl;
			break;
        }
    }
    delete socket;
};

template <class T, class S>
zmq::InEventPort<T, S>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function):
::InEventPort<T>(component, name, callback_function, "zmq"){
    terminate_endpoint_ = "inproc://term*" + component->get_name() + "*" + name + "*";
};


template <class T, class S>
void zmq::InEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);
    end_points_.clear();

    if(attributes.count("publisher_address")){
        for(auto s : attributes["publisher_address"]->StringList()){
            end_points_.push_back(s);
        }
    }

    if(!end_points_.empty()){
        configured_ = true;
    }else{
        std::cerr << "zmq::InEventPort<T, S>::startup: No Valid Endpoints" << std::endl;
    }
};


template <class T, class S>
zmq::InEventPort<T, S>::~InEventPort(){
};


template <class T, class S>
bool zmq::InEventPort<T, S>::Teardown(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::Teardown()){
        if(zmq_thread_){
            //Join our zmq_thread
            zmq_thread_->join();
            delete zmq_thread_;
            zmq_thread_ = 0;
        }
        configured_ = false;
        return true;
    }
    return false;
};

template <class T, class S>
bool zmq::InEventPort<T, S>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::Activate()){
        if(configured_){
            zmq_thread_ = new std::thread(&zmq::InEventPort<T, S>::zmq_loop, this);
            
            //Wait for the thread to be actually started
            std::unique_lock<std::mutex> lock(ready_mutex_);
            ready_lock_condition_.wait(lock);
            EventPort::LogActivation();
        }
        
        return true;
    }
    return false;
};

template <class T, class S>
bool zmq::InEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::Passivate()){
        if(zmq_thread_){
            //Construct our terminate socket
            auto helper = ZmqHelper::get_zmq_helper();
            auto term_socket = helper->get_publisher_socket();
            term_socket->bind(terminate_endpoint_.c_str());

            //Send a blank message to interupt the recv loop
            term_socket->send(zmq::message_t());
            delete term_socket;
            EventPort::LogPassivation();
        }
        return true;
    }
    return false;
};





#endif //ZMQ_INEVENTPORT_H
