#ifndef ZMQ_INEVENTPORT_H
#define ZMQ_INEVENTPORT_H

#include "../core/eventports/ineventport.hpp"
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
            void Teardown();

            bool Activate();
            bool Passivate();

        private:
            void receive_loop();
            void zmq_loop();
            
            

            std::thread* zmq_thread_ = 0;
            std::thread* rec_thread_ = 0;
            std::string terminate_endpoint_;

            std::vector<std::string> end_points_;

            std::mutex notify_mutex_;
            std::mutex control_mutex_;
            std::condition_variable notify_lock_condition_;
            std::queue<std::string> message_queue_;

            bool configured_ = false;
    }; 
};

template <class T, class S>
void zmq::InEventPort<T, S>::receive_loop(){
    std::queue<std::string> queue_;

    while(true){
        {
            //Wait for next message
            std::unique_lock<std::mutex> lock(notify_mutex_);
            notify_lock_condition_.wait(lock);
            //Swap out the queue's and release the mutex
            message_queue_.swap(queue_);
        }

        while(!queue_.empty()){
            std::string str = queue_.front();
            //auto s = new S();
            auto m = proto::decode<S>(str);
            //delete s;
            this->rx(m);
            queue_.pop();
        }
    }
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
        std::cout << "zmq::InEventPort<T, S>::zmq_loop(): Couldn't connect to endpoints!" << std::endl;
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
                //Gain mutex lock and append message
                std::unique_lock<std::mutex> lock(notify_mutex_);
                message_queue_.push(msg_str);
                notify_lock_condition_.notify_all();
            }else{
                //Got Terminate message, length 0
                break;
            }
        }catch(zmq::error_t ex){
            //Do nothing with an error.
            std::cout << "zmq::InEventPort<T, S>::zmq_loop(): ZMQ_Error: " << ex.num() << std::endl;
			break;
        }
    }
    delete socket;
};

template <class T, class S>
zmq::InEventPort<T, S>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function):
::InEventPort<T>(component, name, callback_function){
    terminate_endpoint_ = "inproc://term*" + component->get_name() + "*" + name + "*";
    std::cout << "zmq::InEventPort<T, S>::InEventPort: Terminate Endpoint: " << terminate_endpoint_ << std::endl;
};


template <class T, class S>
void zmq::InEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){

    std::lock_guard<std::mutex> lock(control_mutex_);
    end_points_.clear();

    if(attributes.count("publisher_address")){
        for(auto s : attributes["publisher_address"]->s){
            end_points_.push_back(s);
        }
    }

    if(!end_points_.empty()){
        rec_thread_ = new std::thread(&zmq::InEventPort<T, S>::receive_loop, this);
        configured_=true;
    }else{
        std::cout << "zmq::InEventPort<T, S>::startup: No Valid Reciever Endpoints" << std::endl;
    }
};




template <class T, class S>
zmq::InEventPort<T, S>::~InEventPort(){
    std::cout << "zmq::InEventPort<T, S>::~InEventPort()" << std::endl;
};


template <class T, class S>
void zmq::InEventPort<T, S>::Teardown(){
    Passivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
};

template <class T, class S>
bool zmq::InEventPort<T, S>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(configured_){
        zmq_thread_ = new std::thread(&zmq::InEventPort<T, S>::zmq_loop, this);
    }
    return ::InEventPort<T>::Activate();
};

template <class T, class S>
bool zmq::InEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(zmq_thread_){
        //Construct our terminate socket
        auto helper = ZmqHelper::get_zmq_helper();
        auto term_socket = helper->get_publisher_socket();
	    term_socket->bind(terminate_endpoint_.c_str());

        //Send a blank message to interupt the recv loop
        term_socket->send(zmq::message_t());

        //Join our zmq_thread
        zmq_thread_->join();
        delete zmq_thread_;
        zmq_thread_ = 0;
        delete term_socket;
    }
    
    return ::InEventPort<T>::Passivate();
};

#endif //ZMQ_INEVENTPORT_H
