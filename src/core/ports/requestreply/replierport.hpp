#ifndef BASE_PORT_REPLIER_HPP
#define BASE_PORT_REPLIER_HPP

#include "../port.h"
#include "../../component.h"

//Generic templated ReplierPort
template <class BaseReplyType, class BaseRequestType>
class ReplierPort : public Port{
    public:
        ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper, const std::string& middleware);

        using base_reply_type = BaseReplyType;
        using base_request_type = BaseRequestType;
        BaseReplyType ProcessRequest(BaseRequestType& type);
    private:
        const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper_;
};

//Specialised templated ReplierPort for void returning
template <class BaseRequestType>
class ReplierPort<void, BaseRequestType> : public Port{
    public:
        ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<void, BaseRequestType>& callback_wrapper, const std::string& middleware);
        using base_reply_type = void;
        using base_request_type = BaseRequestType;
        void ProcessRequest(BaseRequestType& type);
    protected:
    private:
        const CallbackWrapper<void, BaseRequestType>& callback_wrapper_;
};

//Specialised templated ReplierPort for void returning
template <class BaseReplyType>
class ReplierPort<BaseReplyType, void> : public Port{
    public:
        ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<BaseReplyType, void>& callback_wrapper, const std::string& middleware);
        using base_reply_type = BaseReplyType;
        using base_request_type = void;
        BaseReplyType ProcessRequest();
    protected:
    private:
        const CallbackWrapper<BaseReplyType, void>& callback_wrapper_;
};

//Generic templated ReplierPort
template <class BaseReplyType, class BaseRequestType>
ReplierPort<BaseReplyType, BaseRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<BaseReplyType, BaseRequestType>& callback_wrapper, const std::string& middleware)
:   Port(component, port_name, Port::Kind::REPLIER, middleware),
    callback_wrapper_(callback_wrapper)
{

};

template <class BaseReplyType, class BaseRequestType>
BaseReplyType ReplierPort<BaseReplyType, BaseRequestType>::ProcessRequest(BaseRequestType& base_request){
    EventRecieved(base_request);
    auto process_message = is_running() && callback_wrapper_.callback_fn;
    if(process_message){
        logger().LogComponentEvent(*this, base_request, Logger::ComponentEvent::STARTED_FUNC);
        auto base_reply = callback_wrapper_.callback_fn(base_request);
        logger().LogComponentEvent(*this, base_request, Logger::ComponentEvent::FINISHED_FUNC);
        EventProcessed(base_request);
        return base_reply;
    }
    EventIgnored(base_request);
    return BaseReplyType();
};

//Specialised templated ReplierPort for void returning
template <class BaseRequestType>
ReplierPort<void, BaseRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<void, BaseRequestType>& callback_wrapper, const std::string& middleware)
:   Port(component, port_name, Port::Kind::REPLIER, middleware),
    callback_wrapper_(callback_wrapper)
{};

template <class BaseRequestType>
void ReplierPort<void, BaseRequestType>::ProcessRequest(BaseRequestType& base_request){
    EventRecieved(base_request);
    auto process_message = is_running() && callback_wrapper_.callback_fn;
    if(process_message){
        logger().LogComponentEvent(*this, base_request, Logger::ComponentEvent::STARTED_FUNC);
        callback_wrapper_.callback_fn(base_request);
        logger().LogComponentEvent(*this, base_request, Logger::ComponentEvent::FINISHED_FUNC);
        EventProcessed(base_request);
        return;
    }
    EventIgnored(base_request);
};



//Specialised templated ReplierPort for void requesting
template <class BaseReplyType>
ReplierPort<BaseReplyType, void>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, const CallbackWrapper<BaseReplyType, void>& callback_wrapper, const std::string& middleware)
:   Port(component, port_name, Port::Kind::REPLIER, middleware),
    callback_wrapper_(callback_wrapper)
{};

template <class BaseReplyType>
BaseReplyType ReplierPort<BaseReplyType, void>::ProcessRequest(){
    BaseMessage base_request;
    EventRecieved(base_request);
    auto process_message = is_running() && callback_wrapper_.callback_fn;
    if(process_message){
        logger().LogComponentEvent(*this, base_request, Logger::ComponentEvent::STARTED_FUNC);
        auto base_reply = callback_wrapper_.callback_fn();
        logger().LogComponentEvent(*this, base_request, Logger::ComponentEvent::FINISHED_FUNC);
        EventProcessed(base_request);
        return base_reply;
    }
    EventIgnored(base_request);
    return BaseReplyType();
};

#endif // BASE_PORT_REPLIER_HPP