#ifndef QPID_OUTEVENTPORT_H
#define QPID_OUTEVENTPORT_H

#include <core/eventports/prototranslator.h>
#include <core/eventports/ineventport.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <mutex>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Message_io.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Duration.h>

namespace qpid{
    template <class T, class S> class OutEventPort: public ::OutEventPort<T>{
        public:
            OutEventPort(std::weak_ptr<Component> component, std::string name);
           ~OutEventPort(){
                Activatable::Terminate();
            }
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        public:
            bool tx(const T& message);
        private:
            bool setup_tx();

            std::mutex control_mutex_;
            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Sender sender_ = 0;

        
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;

    };
};


template <class T, class S>
qpid::OutEventPort<T, S>::OutEventPort(std::weak_ptr<Component> component, std::string name):
::OutEventPort<T>(component, name, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};


template <class T, class S>
bool qpid::OutEventPort<T, S>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool valid = topic_name_->String().length() >= 0 && broker_->String().length() >= 0;
    if(valid && ::OutEventPort<T>::HandleConfigure()){
        return setup_tx();
    }
    return false;
};

template <class T, class S>
bool qpid::OutEventPort<T, S>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::OutEventPort<T>::HandlePassivate()){
        if(connection_ && connection_.isOpen()){
            connection_.close();
            connection_ = 0;
            sender_ = 0;
        }
        return true;
    }
    return false;
};

template <class T, class S>
bool qpid::OutEventPort<T, S>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    return ::OutEventPort<T>::HandleTerminate();
};

template <class T, class S>
bool qpid::OutEventPort<T, S>::tx(const T& message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool should_send = ::OutEventPort<T>::tx(message);

    if(should_send && sender_){
        auto str = proto::encode(message);
        qpid::messaging::Message m;
        m.setContentObject(str);
        sender_.send(m);
        return true;
    }
    return false;
};


template <class T, class S>
bool qpid::OutEventPort<T, S>::setup_tx(){
    try{
        if(!connection_){
            connection_ = qpid::messaging::Connection(broker_->String());
            connection_.open();
            auto session = connection_.createSession();
            std::string tn = "amq.topic/" + topic_name_->String();
            sender_ = session.createSender(tn);
            return true;
        }
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to startup QPID Sender") + ex.what());
    }
    return false;
};

#endif //QPID_OUTEVENTPORT_H
