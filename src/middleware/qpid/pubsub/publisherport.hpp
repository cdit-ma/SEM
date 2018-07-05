#ifndef QPID_PORT_PUBLISHER_HPP
#define QPID_PORT_PUBLISHER_HPP

#include <middleware/proto/prototranslator.h>
#include <core/ports/pubsub/publisherport.hpp>
#include <middleware/qpid/qpidhelper.h>

#include <mutex>
#include <iostream>
#include <string>
#include <mutex>

namespace qpid{
    template <class BaseType, class ProtoType> class PublisherPort: public ::PublisherPort<BaseType>{
        public:
            PublisherPort(std::weak_ptr<Component> component, std::string name);
            ~PublisherPort(){this->Terminate()};
            void Send(const BaseType& message);
        protected:
            void HandleConfigure();
            void HandlePassivate();
            void HandleTerminate();
        private:
            ::Proto::Translator<BaseType, ProtoType> translator;

            std::mutex mutex_;
            std::unique_ptr<PortHelper> port_helper_;
            
            std::shared_ptr<Attribute> topic_name_;
            std::shared_ptr<Attribute> broker_;
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
    std::lock_guard<std::mutex> lock(mutex_);
    if(port_helper_)
        throw std::runtime_error("qpid PublisherPort Port: '" + this->get_name() + "': Has an errant Port Helper!");

    port_helper_ =  std::unique_ptr<PortHelper>(new PortHelper(broker_->String()));
    ::PublisherPort<BaseType>::HandleConfigure();
};

template <class BaseType, class ProtoType>
void qpid::PublisherPort<BaseType, ProtoType>::HandlePassivate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(port_helper_){
        port_helper_->Terminate();
    }
    ::PublisherPort<BaseType>::HandlePassivate();
};

template <class BaseType, class ProtoType>
bool qpid::PublisherPort<BaseType, ProtoType>::HandleTerminate(){
    port_helper_.reset();
    ::PublisherPort<BaseType>::HandleTerminate();
};

template <class BaseType, class ProtoType>
void qpid::PublisherPort<BaseType, ProtoType>::Send(const BaseType& message){
    //Log the recieving
    this->EventRecieved(message);

    if(this->is_running()){
        qpid::messaging::Sender sender = 0;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if(port_helper_){
                sender = port_helper_->GetSender(topic_name_->String());
            }
        }

        if(sender){
            const auto& str = translator.BaseToString(message);
            sender_.send(qpid::messaging::Message(str));
            this->EventProcessed(message);
            this->logger().LogComponentEvent(*this, message, ModelLogger::ComponentEvent::SENT);
        }
    }
    this->EventIgnored(message);
};

#endif //QPID_PORT_PUBLISHER_HPP
