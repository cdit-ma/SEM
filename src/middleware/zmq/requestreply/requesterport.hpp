#ifndef ZMQ_PORT_REQUESTER_HPP
#define ZMQ_PORT_REQUESTER_HPP

#include <core/ports/requestreply/requesterport.hpp>
#include <middleware/proto/prototranslator.h>
#include <middleware/zmq/zmqhelper.h>
#include <re_common/zmq/zmqutils.hpp>

namespace zmq{
    template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
    class RequesterPort : public ::RequesterPort<BaseReplyType, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            }
            BaseReplyType* ProcessRequest(const BaseRequestType& type, std::chrono::milliseconds timeout);
        private:
            std::shared_ptr<Attribute> end_point_;

            //Translators
            ::Proto::Translator<BaseReplyType, ReplyType> reply_translator;
            ::Proto::Translator<BaseRequestType, RequestType> request_translator;
    }; 
};

template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
zmq::RequesterPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, BaseRequestType>(component, port_name, "zmq"){
    end_point_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseReplyType, class ReplyType, class BaseRequestType, class RequestType>
BaseReplyType* zmq::RequesterPort<BaseReplyType, ReplyType, BaseRequestType, RequestType>::ProcessRequest(const BaseRequestType& request, std::chrono::milliseconds timeout){
    //Get the address
    const auto address = end_point_->String();
    try{
        auto helper = ZmqHelper::get_zmq_helper();
        auto socket = helper->get_request_socket();
        
        //Connect to the address
        socket.connect(address.c_str());

        const auto request_str = request_translator.BaseToString(request);
        zmq::message_t zmq_request(request_str.c_str(), request_str.size());
        socket.send(zmq_request);

        //Poll
        auto events = ZmqHelper::get_zmq_helper()->poll_socket(socket, timeout);

        if(events == 0){
            throw std::runtime_error("Timeout");
        }

        zmq::message_t zmq_reply;
        socket.recv(&zmq_reply);
        const auto reply_str = Zmq2String(zmq_reply);
        
        return reply_translator.StringToBase(reply_str);
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to endpoint: '" + address + "' " + ex.what());
    }
    return 0;
};

#endif //ZMQ_PORT_REQUESTER_HPP
