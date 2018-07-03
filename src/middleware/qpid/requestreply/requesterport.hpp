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
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;

            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Session session_ = 0;
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
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;

            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Session session_ = 0;

    };

    //Specialised templated RequesterPort for void requesting
    template <class BaseReplyType, class ProtoReplyType>
    class RequesterPort<BaseReplyType, ProtoReplyType, void, void> : public ::RequesterPort<BaseReplyType, void>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            }
            BaseReplyType ProcessRequest(std::chrono::milliseconds timeout);
            
            using middleware_reply_type = BaseReplyType;
            using middleware_request_type = void;
        protected:
            bool HandleConfigure();
            bool HandlePassivate();
            bool HandleTerminate();
        private:
            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;

            qpid::messaging::Connection connection_ = 0;
            qpid::messaging::Session session_ = 0;
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
bool qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    try{
        bool valid = topic_name_->String().length() >= 0 && broker_->String().length() >= 0;
        if(valid && ::RequesterPort<BaseReplyType, BaseRequestType>::HandleConfigure()){
            if(!connection_){
                //Construct qpid connection and session with broker info and constructed topic name
                connection_ = qpid::messaging::Connection(broker_->String());
                connection_.open();
                session_ = connection_.createSession();
                return true;
            }
        }
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to startup QPID requester") + ex.what());
    }
    return false;
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandlePassivate(){
    if(::RequesterPort<BaseReplyType, BaseRequestType>::HandlePassivate()){
        if(connection_ && connection_.isOpen()){
            connection_.close();
            connection_ = 0;
            session_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
bool qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::HandleTerminate(){
    HandlePassivate();
    return ::RequesterPort<BaseReplyType, BaseRequestType>::HandleTerminate();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
BaseReplyType qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout){
    try{


        //Construct sender
        auto sender = session_.createSender("amq.topic/reqrep/"  + topic_name_->String());

        //Construct receiver
        auto response_queue_address = qpid::messaging::Address("#response-queue; {create: always, delete:always}");
        auto receiver = session_.createReceiver(response_queue_address);
        //Translate the base_request object into a string
        const auto request_str = ::Proto::Translator<BaseRequestType, ProtoRequestType>::BaseToString(base_request);

        //Convert request
        qpid::messaging::Message qpid_request(request_str);
        qpid_request.setReplyTo(response_queue_address);

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
    }
    catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(ex.what());
        throw;
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
bool qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::HandleConfigure(){
    try{
        bool valid = topic_name_->String().length() >= 0 && broker_->String().length() >= 0;
        if(valid && ::RequesterPort<void, BaseRequestType>::HandleConfigure()){
            if(!connection_){
                //Construct qpid connection and session with broker info and constructed topic name
                connection_ = qpid::messaging::Connection(broker_->String());
                connection_.open();
                session_ = connection_.createSession();
                return true;
            }
        }
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to startup QPID requester") + ex.what());
    }
    return false;
};

template <class BaseRequestType, class ProtoRequestType>
bool qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::HandlePassivate(){
    if(::RequesterPort<void, BaseRequestType>::HandlePassivate()){
        if(connection_ && connection_.isOpen()){
            connection_.close();
            connection_ = 0;
            session_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseRequestType, class ProtoRequestType>
bool qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::HandleTerminate(){
    HandlePassivate();
    return ::RequesterPort<void, BaseRequestType>::HandleTerminate();
};

template <class BaseRequestType, class ProtoRequestType>
void qpid::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout){
    try{
        //Construct sender
        auto sender = session_.createSender("amq.topic/reqrep/"  + topic_name_->String());

        //Construct receiver
        auto response_queue_address = qpid::messaging::Address("#response-queue; {create: always, delete:always}");
        auto receiver = session_.createReceiver(response_queue_address);

        //Translate the base_request object into a string
        const auto request_str = ::Proto::Translator<BaseRequestType, ProtoRequestType>::BaseToString(base_request);

        //Convert request
        qpid::messaging::Message qpid_request(request_str);
        qpid_request.setReplyTo(response_queue_address);

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
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(ex.what());
        throw;
    }
};






//Specialised templated RequesterPort for void requesting
template <class BaseReplyType, class ProtoReplyType>
qpid::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, void>(component, port_name, "qpid"){

    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseReplyType, class ProtoReplyType>
bool qpid::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::HandleConfigure(){
    try{
        bool valid = topic_name_->String().length() >= 0 && broker_->String().length() >= 0;
        if(valid && ::RequesterPort<BaseReplyType, void>::HandleConfigure()){
            if(!connection_){
                //Construct qpid connection and session with broker info and constructed topic name
                connection_ = qpid::messaging::Connection(broker_->String());
                connection_.open();
                session_ = connection_.createSession();

                return true;
            }
        }
    }catch(const std::exception& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(std::string("Unable to startup QPID requester") + ex.what());
    }
    return false;
};

template <class BaseReplyType, class ProtoReplyType>
bool qpid::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::HandlePassivate(){
    if(::RequesterPort<BaseReplyType, void>::HandlePassivate()){
        if(connection_ && connection_.isOpen()){
            connection_.close();
            connection_ = 0;
            session_ = 0;
        }
        return true;
    }
    return false;
};

template <class BaseReplyType, class ProtoReplyType>
bool qpid::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::HandleTerminate(){
    HandlePassivate();
    return ::RequesterPort<BaseReplyType, void>::HandleTerminate();
};

template <class BaseReplyType, class ProtoReplyType>
BaseReplyType qpid::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::ProcessRequest(std::chrono::milliseconds timeout){
    try{
        //Construct sender
        auto sender = session_.createSender("amq.topic/reqrep/"  + topic_name_->String());

        //Construct receiver
        auto response_queue_address = qpid::messaging::Address("#response-queue; {create: always, delete:always}");
        auto receiver = session_.createReceiver(response_queue_address);

        //Convert request
        qpid::messaging::Message qpid_request;
        qpid_request.setReplyTo(response_queue_address);

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
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg(ex.what());
        throw;
    }
};

#endif //QPID_PORT_REQUESTER_HPP