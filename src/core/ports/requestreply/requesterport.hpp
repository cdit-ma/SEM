#ifndef BASE_PORT_REQUESTER_HPP
#define BASE_PORT_REQUESTER_HPP

#include "../port.h"
#include "../../modellogger.h"
#include "../../component.h"

template <class BaseReplyType, class BaseRequestType>
class RequesterPort : public Port{
    public:
        RequesterPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware);
        std::pair<bool, BaseReplyType> SendRequest(const BaseRequestType& request, std::chrono::milliseconds timeout);
    protected:
        virtual BaseReplyType* ProcessRequest(const BaseRequestType& type, std::chrono::milliseconds timeout) = 0;
};

template  <class BaseReplyType, class BaseRequestType>
RequesterPort<BaseReplyType, BaseRequestType>::RequesterPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
: Port(component, port_name, Port::Kind::REQUESTER, middleware){

};

template  <class BaseReplyType, class BaseRequestType>
std::pair<bool, BaseReplyType> RequesterPort<BaseReplyType, BaseRequestType>::SendRequest(const BaseRequestType& request, std::chrono::milliseconds timeout)
{
    std::pair<bool, BaseReplyType> response;
    response.first = false;
    try{
        auto reply_message = ProcessRequest(request, timeout);
        if(reply_message){
            response.first = true;
            response.second = BaseReplyType(*reply_message);
            delete reply_message;
        }
    }catch(const std::exception& e){
        
    }
    return response;
};

#endif // BASE_PORT_REQUESTER_HPP
