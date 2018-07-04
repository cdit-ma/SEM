#ifndef ZMQ_PORT_REQUESTER_HPP
#define ZMQ_PORT_REQUESTER_HPP

#include <core/ports/requestreply/requesterport.hpp>
#include <middleware/proto/prototranslator.h>
#include <middleware/zmq/zmqhelper.h>
#include <re_common/zmq/zmqutils.hpp>

namespace zmq{
    //Generic templated RequesterPort
    template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
    class RequesterPort : public ::RequesterPort<BaseReplyType, BaseRequestType>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            };
            BaseReplyType ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout);
            
            
            using middleware_reply_type = ProtoReplyType;
            using middleware_request_type = ProtoRequestType;
        private:
            std::shared_ptr<Attribute> server_address_;
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
            std::shared_ptr<Attribute> server_address_;
    };

    //Specialised templated RequesterPort for void returning
    template <class BaseReplyType, class ProtoReplyType>
    class RequesterPort<BaseReplyType, ProtoReplyType, void, void> : public ::RequesterPort<BaseReplyType, void>{
        public:
            RequesterPort(std::weak_ptr<Component> component, const std::string& port_name);
            ~RequesterPort(){
                Activatable::Terminate();
            };
            BaseReplyType ProcessRequest(std::chrono::milliseconds timeout);
            
            
            using middleware_reply_type = ProtoReplyType;
            using middleware_request_type = void;
        private:
            std::shared_ptr<Attribute> server_address_;
    };
};

//Generic templated RequesterPort
template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
zmq::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, BaseRequestType>(component, port_name, "zmq"){
    server_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseReplyType, class ProtoReplyType, class BaseRequestType, class ProtoRequestType>
BaseReplyType zmq::RequesterPort<BaseReplyType, ProtoReplyType, BaseRequestType, ProtoRequestType>::ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout){
    const auto address = server_address_->String();
    try{
        auto& helper = ZmqHelper::get_zmq_helper();
        auto socket = helper.get_request_socket();
        
        //Connect to the address
        socket->connect(address.c_str());

        //Translate the base_request object into a string
        const auto request_str = ::Proto::Translator<BaseRequestType, ProtoRequestType>::BaseToString(base_request);
        
        //Send the request
        socket->send(String2Zmq(request_str));

        //Poll for our timeout
        auto events = helper.poll_socket(*socket, timeout);

        if(events == 0){
            throw std::runtime_error("Timeout");
        }

        //Block and wait for the reply message
        zmq::message_t zmq_reply;
        socket->recv(&zmq_reply);
        
        //Get the string
        const auto reply_str = Zmq2String(zmq_reply);
        
        //Translate the string to a base_reply_ptr
        auto base_reply_ptr = ::Proto::Translator<BaseReplyType, ProtoReplyType>::StringToBase(reply_str);
        
        //Copy the message into a heap allocated object
        BaseReplyType base_reply(*base_reply_ptr);
        
        //Clean up the memory from the base_reply_ptr
        delete base_reply_ptr;

        //Return the reply object
        return base_reply;
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to endpoint: '" + address + "' " + ex.what());
        throw std::runtime_error("Cannot connect to endpoint: '" + address + "' ");
    }
};

//Specialised templated RequesterPort for void returning
template <class BaseRequestType, class ProtoRequestType>
zmq::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<void, BaseRequestType>(component, port_name, "zmq"){
    server_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseRequestType, class ProtoRequestType>
void zmq::RequesterPort<void, void, BaseRequestType, ProtoRequestType>::ProcessRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout){
    const auto address = server_address_->String();
    try{
        auto& helper = ZmqHelper::get_zmq_helper();
        auto socket = helper.get_request_socket();
        
        //Connect to the address
        socket->connect(address.c_str());

        //Translate the base_request object into a string
        const auto request_str = ::Proto::Translator<BaseRequestType, ProtoRequestType>::BaseToString(base_request);
        
        //Send the request
        socket->send(String2Zmq(request_str));

        //Poll for our timeout
        auto events = helper.poll_socket(*socket, timeout);

        if(events == 0){
            throw std::runtime_error("Timeout waiting for response");
        }

        //Block and wait for the reply message
        zmq::message_t zmq_reply;
        socket->recv(&zmq_reply);
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to endpoint: '" + address + "' " + ex.what());
        throw std::runtime_error("Cannot connect to endpoint: '" + address + "' ");
    }
};

//Specialised templated RequesterPort for void returning
template <class BaseReplyType, class ProtoReplyType>
zmq::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name):
::RequesterPort<BaseReplyType, void>(component, port_name, "zmq"){
    server_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "server_address").lock();
};

template <class BaseReplyType, class ProtoReplyType>
BaseReplyType zmq::RequesterPort<BaseReplyType, ProtoReplyType, void, void>::ProcessRequest(std::chrono::milliseconds timeout){
    const auto address = server_address_->String();
    try{
        auto& helper = ZmqHelper::get_zmq_helper();
        auto socket = helper.get_request_socket();
        
        //Connect to the address
        socket->connect(address.c_str());
        
        //Send the blank request
        socket->send(zmq::message_t());

        //Poll for our timeout
        auto events = helper.poll_socket(*socket, timeout);

        if(events == 0){
            throw std::runtime_error("Timeout waiting for response");
        }

        //Block and wait for the reply message
        zmq::message_t zmq_reply;
        socket->recv(&zmq_reply);

        //Get the string
        const auto reply_str = Zmq2String(zmq_reply);
        
        //Translate the string to a base_reply_ptr
        auto base_reply_ptr = ::Proto::Translator<BaseReplyType, ProtoReplyType>::StringToBase(reply_str);
        
        //Copy the message into a heap allocated object
        BaseReplyType base_reply(*base_reply_ptr);
        
        //Clean up the memory from the base_reply_ptr
        delete base_reply_ptr;

        //Return the reply object
        return base_reply;
    }catch(const zmq::error_t& ex){
        Log(Severity::ERROR_).Context(this).Func(__func__).Msg("Cannot connect to endpoint: '" + address + "' " + ex.what());
        throw std::runtime_error("Cannot connect to endpoint: '" + address + "' ");
    }
};

#endif //ZMQ_PORT_REQUESTER_HPP