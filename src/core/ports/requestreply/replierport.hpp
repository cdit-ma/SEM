#ifndef BASE_PORT_REPLIER_HPP
#define BASE_PORT_REPLIER_HPP

#include "../port.h"
#include "../../modellogger.h"
#include "../../component.h"

//Generic templated ReplierPort
template <class BaseReplyType, class BaseRequestType>
class ReplierPort : public Port{
    public:
        ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > callback_function, const std::string& middleware);

        using base_reply_type = BaseReplyType;
        using base_request_type = BaseRequestType;
        BaseReplyType ProcessRequest(BaseRequestType& type);
    protected:
    private:
        std::function<BaseReplyType (BaseRequestType&) > callback_function_;
};

//Specialised templated ReplierPort for void returning
template <class BaseRequestType>
class ReplierPort<void, BaseRequestType> : public Port{
    public:
        ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<void (BaseRequestType&) > callback_function, const std::string& middleware);
        using base_reply_type = void;
        using base_request_type = BaseRequestType;
        void ProcessRequest(BaseRequestType& type);
    protected:
    private:
        std::function<void (BaseRequestType&) > callback_function_;
};

//Generic templated ReplierPort
template <class BaseReplyType, class BaseRequestType>
ReplierPort<BaseReplyType, BaseRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > callback_function, const std::string& middleware)
: Port(component, port_name, Port::Kind::REPLIER, middleware){
    callback_function_ = callback_function;
};

template <class BaseReplyType, class BaseRequestType>
BaseReplyType ReplierPort<BaseReplyType, BaseRequestType>::ProcessRequest(BaseRequestType& base_request){
    EventRecieved(base_request);
    auto process_message = is_running() && callback_function_;
    if(process_message){
        logger()->LogComponentEvent(*this, base_request, ModelLogger::ComponentEvent::STARTED_FUNC);
        auto base_reply = callback_function_(base_request);
        logger()->LogComponentEvent(*this, base_request, ModelLogger::ComponentEvent::FINISHED_FUNC);
        EventProcessed(base_request, process_message);
        return base_reply;
    }
    EventProcessed(base_request, false);
    return BaseReplyType();
};

//Specialised templated ReplierPort for void returning
template <class BaseRequestType>
ReplierPort<void, BaseRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<void (BaseRequestType&) > callback_function, const std::string& middleware)
: Port(component, port_name, Port::Kind::REPLIER, middleware){
    callback_function_ = callback_function;
};

template <class BaseRequestType>
void ReplierPort<void, BaseRequestType>::ProcessRequest(BaseRequestType& base_request){
    EventRecieved(base_request);
    auto process_message = is_running() && callback_function_;
    if(process_message){
        logger()->LogComponentEvent(*this, base_request, ModelLogger::ComponentEvent::STARTED_FUNC);
        callback_function_(base_request);
        logger()->LogComponentEvent(*this, base_request, ModelLogger::ComponentEvent::FINISHED_FUNC);
        EventProcessed(base_request, process_message);
        return;
    }
    EventProcessed(base_request, false);
};

#endif // BASE_PORT_REPLIER_HPP