#ifndef QPID_PORT_PUBLISHER_HPP
#define QPID_PORT_PUBLISHER_HPP

#include <middleware/proto/prototranslator.h>
#include <core/ports/pubsub/publisherport.hpp>

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
    template <class BaseType, class ProtoType> class PublisherPort: public ::PublisherPort<BaseType>{
        public:
            PublisherPort(std::weak_ptr<Component> component, std::string name);
           ~PublisherPort(){
                Activatable::Terminate();
            }
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        public:
            bool Send(const BaseType& message);
        private:
            bool setup_tx();

            ::Proto::Translator<BaseType, ProtoType> translator;

            std::mutex control_mutex_;
            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Sender sender_ = 0;

        
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;

    };
};


template <class BaseType, class ProtoType>
qpid::PublisherPort<BaseType, ProtoType>::PublisherPort(std::weak_ptr<Component> component, std::string name):
::PublisherPort<BaseType>(component, name, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};


template <class BaseType, class ProtoType>
bool qpid::PublisherPort<BaseType, ProtoType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool valid = topic_name_->String().length() >= 0 && broker_->String().length() >= 0;
    if(valid && ::PublisherPort<BaseType>::HandleConfigure()){
        return setup_tx();
    }
    return false;
};

template <class BaseType, class ProtoType>
bool qpid::PublisherPort<BaseType, ProtoType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(control_mutex_);
    if(::PublisherPort<BaseType>::HandlePassivate()){
        if(connection_ && connection_.isOpen()){
            connection_.close();
            connection_ = 0;
            sender_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseType, class ProtoType>
bool qpid::PublisherPort<BaseType, ProtoType>::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> lock(control_mutex_);
    return ::PublisherPort<BaseType>::HandleTerminate();
};

template <class BaseType, class ProtoType>
bool qpid::PublisherPort<BaseType, ProtoType>::Send(const BaseType& message){
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool should_send = ::PublisherPort<BaseType>::Send(message);

    if(should_send && sender_){
        auto str = translator.BaseToString(message);
        qpid::messaging::Message m;
        m.setContentObject(str);
        sender_.send(m);
        return true;
    }
    return false;
};


template <class BaseType, class ProtoType>
bool qpid::PublisherPort<BaseType, ProtoType>::setup_tx(){
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

#endif //QPID_PORT_PUBLISHER_HPP
