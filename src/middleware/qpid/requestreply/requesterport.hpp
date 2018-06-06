#ifndef QPID_PORT_REQUESTER_HPP
#define QPID_PORT_REQUESTER_HPP

#include <core/ports/requestreply/requesterport.hpp>
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
            ~RequesterPort(){
                Activatable::Terminate();
            }
            BaseReplyType ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);

            using middleware_reply_type = ProtoReplyType;
            using middleware_request_type = ProtoRequestType;
        private:
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;
    };


    //Specialised templated RequesterPort for void returning
    template <class BaseRequestType, class ProtoRequestType>
    class RequesterPort<void, void, BaseRequestType, ProtoRequestType> : public ::RequesterPort<void, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            }
            void ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);
            
            using middleware_reply_type = void;
            using middleware_request_type = ProtoRequestType;
        private:
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;
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
BaseReplyType qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout){
    try{
        //Construct qpid connection and session with broker info and constructed topic name
        qpid::messaging::Connection connection(broker_->String());
        connection.open();
        auto session = connection.createSession();
        auto sender = session.createSender("amq.topic/reqrep/"  + topic_name_->String());

        //Construct receiver
        qpid::messaging::Address response_queue("#response-queue; {create: always, delete:always}");
        auto receiver = session.createReceiver(response_queue);

        //Translate the base_request object into a string
        const auto request_str = ::Proto::Translator<BaseRequestType, ProtoRequestType>::BaseToString(base_request);

        //Convert request
        qpid::messaging::Message qpid_request(request_str);
        qpid_request.setReplyTo(response_queue);

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
        
        auto base_reply_ptr = ::Proto::Translator<BaseReplyType, ProtoReplyType>::StringToBase(reply_str);

        //Copy the message into a heap allocated object
        BaseReplyType base_reply(*base_reply_ptr);
        
        //Clean up the memory from the base_reply_ptr
        delete base_reply_ptr;

        //Return the reply object
        return base_reply;
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__);
        throw std::runtime_error(ex.what());
    }
};

//Specialised templated RequesterPort for void returning
template <class BaseRequestType, class ProtoRequestType>
qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<void, BaseRequestType>(component, port_name, "qpid"){

    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseRequestType, class ProtoRequestType>
void qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout){
    try{
        //Construct qpid connection and session with broker info and constructed topic name
        qpid::messaging::Connection connection(broker_->String());
        connection.open();
        auto session = connection.createSession();
        auto sender = session.createSender("amq.topic/reqrep/"  + topic_name_->String());

        //Construct receiver
        qpid::messaging::Address response_queue("#response-queue; {create: always, delete:always}");
        auto receiver = session.createReceiver(response_queue);

        //Translate the base_request object into a string
        const auto request_str = ::Proto::Translator<BaseRequestType, ProtoRequestType>::BaseToString(base_request);

        //Convert request
        qpid::messaging::Message qpid_request(request_str);
        qpid_request.setReplyTo(response_queue);

        //Send the request
        sender.send(qpid_request);

        //Convert -1 timeout to qpid forever duration
        qpid::messaging::Duration qpid_timeout(timeout.count());
        if(timeout.count() == -1){
            qpid_timeout = qpid::messaging::Duration::FOREVER;
        }

        //Get the reply message
        qpid::messaging::Message qpid_reply = receiver.fetch(qpid_timeout);
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__);
        throw std::runtime_error(ex.what());
    }
};

#endif //QPID_PORT_REQUESTER_HPP