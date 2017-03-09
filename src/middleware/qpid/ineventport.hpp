#ifndef QPID_INEVENTPORT_H
#define QPID_INEVENTPORT_H

#include "../../core/eventports/ineventport.hpp"
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <functional>


#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Session.h>


namespace qpid{
    template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function);

            void Startup(std::map<std::string, ::Attribute*> attributes);
            void Teardown();

            bool Activate();
            bool Passivate();
        private:
            void qpid_loop();

            qpid::messaging::Connection connection_;
            qpid::messaging::Session session_;
            qpid::messaging::Receiver receiver_;

            std::mutex control_mutex_;
            std::mutex notify_mutex_;
            std::thread* receive_thread_;
            std::thread* qpid_thread_;
            std::condition_variable notify_lock_condition_;

            std::queue<std::string> message_queue_;

            bool configured_ = false;
    };
};

template <class T, class S>
qpid::InEventPort<T, S>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function)
: ::InEventPort<T>(component, name, callback_function){
};

template <class T, class S>
void qpid::InEventPort<T, S>::Startup(std::map<std::string, ::Attribute*> attributes){
    std::lock_guard<std::mutex> lock(control_mutex_);

    std::string broker;
    std::string topic_name;

    if(attributes.count("broker")){
        broker = attributes["broker"]->get_String();
    }
    if(attributes.count("topic_name")){
        topic_name = attributes["topic_name"]->get_String();
    }

    std::cout << "qpid::InEventPort" << std::endl;
    std::cout << "**broker: "<< broker << std::endl;
    std::cout << "**topic_name: "<< topic_name << std::endl << std::endl;


    if(broker.length() > 0 && topic_name.length() > 0){
        connection_ = qpid::messaging::Connection(broker);
        connection_.open();
        session_ = connection_.createSession();
        //TODO: fix this to use actual topic name
        receiver_ = session_.createReceiver( "amq.topic/"  + topic_name);
        
        configured_ = true; 
    }
};

template <class T, class S>
void qpid::InEventPort<T, S>::Teardown(){
    Passivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    configured_ = false;
};

template <class T, class S>
bool qpid::InEventPort<T, S>::Activate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(configured_){
        qpid_thread_ = new std::thread(&qpid::InEventPort<T,S>::qpid_loop, this);
    }
    return ::InEventPort<T>::Activate();
};

template <class T, class S>
bool qpid::InEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(qpid_thread_ && connection_.isOpen()){
        //do passivation things here
        receiver_.close();

        qpid_thread_->join();
        delete qpid_thread_;
        qpid_thread_ = 0;
        connection_.close();
    }
    return ::InEventPort<T>::Passivate();
};


template <class T, class S>
void qpid::InEventPort<T, S>::qpid_loop(){
    while(true){
        try{

            auto sample = receiver_.fetch();
            if(receiver_.isClosed()){
                break;
            }
            std::string str = sample.getContent();
            this->EnqueueMessage(str);
        } catch (...){
            if(receiver_.isClosed()){
                break;
            }
        }
    }
};

#endif //QPID_INEVENTPORT_H
