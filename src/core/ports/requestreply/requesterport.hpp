#ifndef BASE_PORT_REQUESTER_HPP
#define BASE_PORT_REQUESTER_HPP

#include "../port.h"
#include "../../modellogger.h"
#include "../../component.h"

//Generic templated RequesterPort
template <class BaseReplyType, class BaseRequestType>
class RequesterPort : public Port{
    public:
        RequesterPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware);
        std::pair<bool, BaseReplyType> SendRequest(const BaseRequestType& request, std::chrono::milliseconds timeout);

        using base_reply_type = BaseReplyType;
        using base_request_type = BaseRequestType;
    protected:
        virtual BaseReplyType ProcessRequest(const BaseRequestType& type, std::chrono::milliseconds timeout) = 0;
};

//Specialised templated RequesterPort for void returning
template <class BaseRequestType>
class RequesterPort<void, BaseRequestType> : public Port{
    public:
        RequesterPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware);
        bool SendRequest(const BaseRequestType& request, std::chrono::milliseconds timeout);
        
        using base_reply_type = void;
        using base_request_type = BaseRequestType;
    protected:
        virtual void ProcessRequest(const BaseRequestType& type, std::chrono::milliseconds timeout) = 0;
};

//Specialised templated RequesterPort for void returning
template <class BaseReplyType>
class RequesterPort<BaseReplyType, void> : public Port{
    public:
        RequesterPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware);
        std::pair<bool, BaseReplyType> SendRequest(std::chrono::milliseconds timeout);
        
        using base_reply_type = BaseReplyType;
        using base_request_type = void;
    protected:
        virtual BaseReplyType ProcessRequest(std::chrono::milliseconds timeout) = 0;
};

//Generic templated RequesterPort
template  <class BaseReplyType, class BaseRequestType>
RequesterPort<BaseReplyType, BaseRequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
: Port(component, port_name, Port::Kind::REQUESTER, middleware){

};

template  <class BaseReplyType, class BaseRequestType>
std::pair<bool, BaseReplyType> RequesterPort<BaseReplyType, BaseRequestType>::SendRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout)
{
    try{
        EventRecieved(base_request);
        auto base_reply = std::move(ProcessRequest(base_request, timeout));
        EventProcessed(base_request);
        return {true, std::move(base_reply)};
    }catch(const std::exception& e){
        EventIgnored(base_request);
    }
    return {false, BaseReplyType()};
};

//Specialised templated RequesterPort for void returning
template  <class BaseRequestType>
RequesterPort<void, BaseRequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
: Port(component, port_name, Port::Kind::REQUESTER, middleware){

};

template  <class BaseRequestType>
bool RequesterPort<void, BaseRequestType>::SendRequest(const BaseRequestType& base_request, std::chrono::milliseconds timeout)
{
    try{
        EventRecieved(base_request);
        ProcessRequest(base_request, timeout);
        EventProcessed(base_request);
        return true;
    }catch(const std::exception& e){
        //std::cerr << e.what() << std::endl;
    }
    EventIgnored(base_request);
    return false;
};


//Specialised templated RequesterPort for void recieving returning
template  <class BaseReplyType>
RequesterPort<BaseReplyType, void>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
: Port(component, port_name, Port::Kind::REQUESTER, middleware){

};

template  <class BaseReplyType>
std::pair<bool, BaseReplyType> RequesterPort<BaseReplyType, void>::SendRequest(std::chrono::milliseconds timeout)
{
    //Generate a request
    try{
        BaseMessage m;
        EventRecieved(m);
        auto base_reply = std::move(ProcessRequest(timeout));
        EventProcessed(m);
        return {true, std::move(base_reply)};
    }catch(const std::exception& e){
        //std::cerr << e.what() << std::endl;
    }
    return {false, BaseReplyType()};
};

#endif // BASE_PORT_REQUESTER_HPP
