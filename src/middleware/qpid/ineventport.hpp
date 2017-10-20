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
#include <qpid/messaging/Duration.h>


namespace qpid{
    template <class T, class S> class InEventPort: public ::InEventPort<T>{
        public:
            InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function);
            ~InEventPort(){
                Passivate();
                Teardown();
            }

            void Startup(std::map<std::string, ::Attribute*> attributes);
            bool Teardown();
            bool Passivate();

        private:
            void qpid_loop();

            qpid::messaging::Connection connection_;
            qpid::messaging::Receiver receiver_;

            std::mutex control_mutex_;
            std::thread* qpid_thread_;
    };
};

template <class T, class S>
qpid::InEventPort<T, S>::InEventPort(Component* component, std::string name, std::function<void (T*) > callback_function)
: ::InEventPort<T>(component, name, callback_function, "qpid"){
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


    if(broker.length() && topic_name.length()){
        //Construct a Qpid Connection
        connection_ = qpid::messaging::Connection(broker);
        //Open the connection
        connection_.open();
        //Construct a session

        auto session = connection_.createSession();
        //TODO: fix this to use actual topic name
        receiver_ = session.createReceiver( "amq.topic/"  + topic_name);

        if(!qpid_thread_){
            qpid_thread_ = new std::thread(&qpid::InEventPort<T,S>::qpid_loop, this);
            Activatable::WaitForStartup();
        }
    }
};

template <class T, class S>
bool qpid::InEventPort<T, S>::Teardown(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::InEventPort<T>::Teardown()){
        if(qpid_thread_){
            qpid_thread_->join();
            delete qpid_thread_;
            qpid_thread_ = 0;
            connection_.close();
        }
        return true;
    }
    return false;
};


template <class T, class S>
bool qpid::InEventPort<T, S>::Passivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);

    if(this->is_active() && connection_.isOpen()){
        //do passivation things here
        receiver_.close();
    }
    return ::InEventPort<T>::Passivate();
};


template <class T, class S>
void qpid::InEventPort<T, S>::qpid_loop(){
    //Notify Startup our thread is good to go
    Activatable::StartupFinished();
    //Wait for the port to be activated before starting!
    Activatable::WaitForActivate();
    //Log the port becoming online
    EventPort::LogActivation();
    while(true){
        try{

            auto sample = receiver_.fetch();
            if(receiver_.isClosed()){
                break;
            }
            std::string str = sample.getContent();
            auto m = proto::decode<S>(str);
            this->EnqueueMessage(m);
        } catch (...){
            if(receiver_.isClosed()){
                break;
            }
        }
    }
    EventPort::LogPassivation();
};

#endif //QPID_INEVENTPORT_H
