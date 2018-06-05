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
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    class RequesterPort : public ::RequesterPort<BaseReplyType, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            }
            BaseReplyType* ProcessRequest(const BaseRequestType& type, std::chrono::milliseconds timeout);
        private:


            //Translators
            ::Proto::Translator<BaseReplyType, ProtoReplyType> reply_translator;
            ::Proto::Translator<BaseRequestType, ProtoRequestType> request_translator;

            std::shared_ptr<Attribute> broker_;
            std::shared_ptr<Attribute> topic_name_;
    };




};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, BaseRequestType>(component, port_name, "qpid"){
    topic_name_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "topic_name").lock();
    broker_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "broker").lock();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
BaseReplyType* qpid::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ProcessRequest(const BaseRequestType& request, std::chrono::milliseconds timeout){
    try{

        qpid::messaging::Connection connection(broker_->String());
        connection.open();
        auto session = connection.createSession();
        auto sender = session.createSender(topic_name_->String());

        qpid::messaging::Address responseQueue("#response-queue; {create: always, delete:always}");
        auto receiver = session.createReceiver(responseQueue);

        qpid::messaging::Message request_message;
        request_message.setReplyTo(responseQueue);
        auto request_str = request_translator.BaseToString(request);
        request_message.setContent(request_str);

        sender.send(request_message);

        qpid::messaging::Message reply_message = receiver.fetch();

        std::string reply_str = reply_message.getContent();
        auto m = reply_translator.StringToBase(reply_str);

        return m;

        
    }catch(const std::exception& ex){
        return 0;
    }
    return 0;
};

#endif //QPID_PORT_REQUESTER_HPP