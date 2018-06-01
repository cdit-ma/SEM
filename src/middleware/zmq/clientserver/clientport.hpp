#ifndef ZMQ_CLIENT_EVENTPORT_H
#define ZMQ_CLIENT_EVENTPORT_H

#include <core/eventports/prototranslator.h>
#include <core/eventports/clientserver/clientport.hpp>

#include <re_common/zmq/zmqutils.hpp>
#include "../zmqhelper.h"

#include <thread>
#include <mutex>

namespace zmq{
    template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
    class ClientEventPort : public ::ClientEventPort<BaseReplyType, BaseRequestType>{
        public:
            ClientEventPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~ClientEventPort(){
                Activatable::Terminate();
            }
            BaseReplyType tx(const BaseRequestType& type);
        protected:
            bool HandleConfigure();
        private:
            std::shared_ptr<Attribute> end_point_;

            //Translators
            ::Proto::Translator<BaseReplyType, ReplyType> reply_translator;
            ::Proto::Translator<BaseRequestType, RequestType> request_translator;
    }; 
};

template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
zmq::ClientEventPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::ClientEventPort(std::weak_ptr<Component> component, const std::string& port_name):
::ClientEventPort<BaseReplyType, BaseRequestType>(component, port_name, "zmq"){
    end_point_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRINGLIST, "server_address").lock();
};

template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
bool zmq::ClientEventPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::HandleConfigure(){
    bool valid = end_point_->StringList().size() > 0;
    return valid;
};

template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
BaseReplyType zmq::ClientEventPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::tx(const BaseRequestType& message){
    auto helper = ZmqHelper::get_zmq_helper();
    auto socket = helper->get_request_socket();

    for(auto e: end_point_->StringList()){
        try{
            //Connect to the address
            socket->connect(e.c_str());
        }catch(zmq::error_t ex){
            Log(Severity::ERROR_).Context(this).Func(GET_FUNC).Msg("Cannot bind endpoint: '" + e + "' " + ex.what());
        }
    }

    auto str = request_translator.BaseToString(message);
    zmq::message_t request_data(str.c_str(), str.size());
    socket->send(request_data);
    
    zmq::message_t reply_data;
    socket->recv(&reply_data);

    auto reply_string = Zmq2String(reply_data);

    auto reply_message = reply_translator.StringToBase(reply_string);
    delete socket;
    return *reply_message;
};

#endif //ZMQ_CLIENT_EVENTPORT_H
