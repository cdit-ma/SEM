#ifndef QPID_PORT_REQUESTER_HPP
#define QPID_PORT_REQUESTER_HPP

#include <core/ports/requestreply/requesterport.hpp>
#include <middleware/qpid/qpidhelper.h>
#include <middleware/proto/prototranslator.h>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Duration.h>
#include <qpid/messaging/Sender.h>

namespace qpid{
    //Generic templated RequesterPort
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    class RequesterPort : public ::RequesterPort<BaseReplyType, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){this->Terminate();};
            
            BaseReplyType ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);

            using middleware_reply_type = ProtoReplyType;
            using middleware_request_type = ProtoRequestType;
        protected:
            void HandleConfigure();
            void HandleTerminate();
        private:
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;

            std::mutex mutex_;
            std::unique_ptr<PortHelper> port_helper_;
    };


    //Specialised templated RequesterPort for void returning
    template <class BaseRequestType, class ProtoRequestType>
    class RequesterPort<void, void, BaseRequestType, ProtoRequestType> : public ::RequesterPort<void, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){this->Terminate();};

            void ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);
            
            using middleware_reply_type = void;
            using middleware_request_type = ProtoRequestType;
        protected:
            void HandleConfigure();
            void HandleTerminate();
        private:
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;

            std::mutex mutex_;
            std::unique_ptr<PortHelper> port_helper_;
    };

    //Specialised templated RequesterPort for void requesting
    template <class BaseReplyType, class ProtoReplyType>
    class RequesterPort<BaseReplyType, ProtoReplyType, void, void> : public ::RequesterPort<BaseReplyType, void>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){this->Terminate();};

            BaseReplyType ProcessRequest(std::chrono::milliseconds timeout);
            
            using middleware_reply_type = BaseReplyType;
            using middleware_request_type = void;
        protected:
            void HandleConfigure();
            void HandleTerminate();
        private:
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;

            std::mutex mutex_;
            std::unique_ptr<PortHelper> port_helper_;
    };
};

//Generic templated RequesterPort
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, BaseRequestType>(component, port_name, "qpid"){

    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(port_helper_)
        throw std::runtime_error("qpid RequesterPort Port: '" + this->get_name() + "': Has an errant Port Helper!");

    port_helper_ =  std::unique_ptr<PortHelper>(new PortHelper(broker_->String()));
    ::RequesterPort<BaseReplyType, BaseRequestType>::HandleConfigure();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
void qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(port_helper_)
        port_helper_->Terminate();
    port_helper_.reset();

    ::RequesterPort<BaseReplyType, BaseRequestType>::HandleTerminate();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
BaseReplyType qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout){
    qpid::messaging::Sender sender = 0;
    qpid::messaging::Receiver receiver = 0;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(port_helper_){
            sender = port_helper_->GetSender(topic_name_->String());
            qpid::messaging::Address receiver_address("#response-queue; {create: always, delete:always}");
            receiver = port_helper_->GetReceiver(receiver_address);
        }
    }
    
    //Translate the base_request object into a string

    try{
        const auto request_str = ::Proto::Translator<BaseRequestType, ProtoRequestType>::BaseToString(base_request);

        //Convert request
        qpid::messaging::Message qpid_request(request_str);
        qpid_request.setReplyTo(receiver.getAddress());

        //Send the request
        sender.send(qpid_request);
    }catch(const std::exception& ex){
        std::string error_str = "Translating Request Failed: ";
        throw std::runtime_error(error_str + ex.what());
    }

    //Convert -1 timeout to qpid forever duration
    qpid::messaging::Duration qpid_timeout(timeout.count());
    if(timeout.count() == -1){
        qpid_timeout = qpid::messaging::Duration::FOREVER;
    }

    //Get the reply message
    qpid::messaging::Message qpid_reply = receiver.fetch(qpid_timeout);
    const auto& reply_str = qpid_reply.getContent();
    
    try{
        auto base_reply_ptr = ::Proto::Translator<BaseReplyType, ProtoReplyType>::StringToBase(reply_str);
        //Copy the message into a heap allocated object
        return BaseReplyType(*base_reply_ptr);
    }catch(const std::exception& ex){
        std::string error_str = "Translating Reply Failed: ";
        throw std::runtime_error(error_str + ex.what());
    }
};



/*
    Specialised template for void response type
*/
template <class BaseRequestType, class ProtoRequestType>
qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<void, BaseRequestType>(component, port_name, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseRequestType, class ProtoRequestType>
void qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(port_helper_)
        throw std::runtime_error("qpid RequesterPort Port: '" + this->get_name() + "': Has an errant Port Helper!");

    port_helper_ =  std::unique_ptr<PortHelper>(new PortHelper(broker_->String()));
    ::RequesterPort<void, BaseRequestType>::HandleConfigure();
};

template <class BaseRequestType, class ProtoRequestType>
void qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(port_helper_)
        port_helper_->Terminate();
    port_helper_.reset();

    ::RequesterPort<void, BaseRequestType>::HandleTerminate();
};

template <class BaseRequestType, class ProtoRequestType>
void qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout){
    qpid::messaging::Sender sender = 0;
    qpid::messaging::Receiver receiver = 0;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(port_helper_){
            sender = port_helper_->GetSender(topic_name_->String());
            qpid::messaging::Address receiver_address("#response-queue; {create: always, delete:always}");
            receiver = port_helper_->GetReceiver(receiver_address);
        }
    }
    try{
        //Translate the base_request object into a string
        const auto request_str = ::Proto::Translator<BaseRequestType, ProtoRequestType>::BaseToString(base_request);
        //Convert request
        qpid::messaging::Message qpid_request(request_str);
        qpid_request.setReplyTo(receiver.getAddress());

        //Send the request
        sender.send(qpid_request);
    }catch(const std::exception& ex){
        std::string error_str = "Translating Request Failed: ";
        throw std::runtime_error(error_str + ex.what());
    }

    //Convert -1 timeout to qpid forever duration
    qpid::messaging::Duration qpid_timeout(timeout.count());
    if(timeout.count() == -1){
        qpid_timeout = qpid::messaging::Duration::FOREVER;
    }

    //Get the reply message
    qpid::messaging::Message qpid_reply = receiver.fetch(qpid_timeout);
};

//Specialised templated RequesterPort for void requesting
template <class BaseReplyType, class ProtoReplyType>
qpid::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, void>(component, port_name, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseReplyType, class ProtoReplyType>
void qpid::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::HandleConfigure(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(port_helper_)
        throw std::runtime_error("qpid RequesterPort Port: '" + this->get_name() + "': Has an errant Port Helper!");

    port_helper_ =  std::unique_ptr<PortHelper>(new PortHelper(broker_->String()));
    ::RequesterPort<BaseReplyType, void>::HandleConfigure();
};

template <class BaseReplyType, class ProtoReplyType>
void qpid::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::HandleTerminate(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(port_helper_)
        port_helper_->Terminate();
    port_helper_.reset();

    ::RequesterPort<BaseReplyType, void>::HandleTerminate();
};

template <class BaseReplyType, class ProtoReplyType>
BaseReplyType qpid::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::ProcessRequest(std::chrono::milliseconds timeout){
    qpid::messaging::Sender sender = 0;
    qpid::messaging::Receiver receiver = 0;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(port_helper_){
            sender = port_helper_->GetSender(topic_name_->String());
            qpid::messaging::Address receiver_address("#response-queue; {create: always, delete:always}");
            receiver = port_helper_->GetReceiver(receiver_address);
        }
    }

    //Convert request
    qpid::messaging::Message qpid_request;
    qpid_request.setReplyTo(receiver.getAddress());

    //Send the request
    sender.send(qpid_request);

    //Convert -1 timeout to qpid forever duration
    qpid::messaging::Duration qpid_timeout(timeout.count());
    if(timeout.count() == -1){
        qpid_timeout = qpid::messaging::Duration::FOREVER;
    }

    //Get the reply message
    qpid::messaging::Message qpid_reply = receiver.fetch(qpid_timeout);
    const auto& reply_str = qpid_reply.getContent();
    
    try{
        auto base_reply_ptr = ::Proto::Translator<BaseReplyType, ProtoReplyType>::StringToBase(reply_str);

        //Copy the message into a heap allocated object
        BaseReplyType base_reply(*base_reply_ptr);

        //Return the reply object
        return base_reply;
    }catch(const std::exception& ex){
        std::string error_str = "Translating Reply Failed: ";
        throw std::runtime_error(error_str + ex.what());
    }
};

#endif //QPID_PORT_REQUESTER_HPP