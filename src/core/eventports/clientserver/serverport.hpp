#ifndef SERVER_EVENT_PORT_HPP
#define SERVER_EVENT_PORT_HPP

#include "../eventport.h"

template <class BaseReplyType, class BaseRequestType> class ServerEventPort : public EventPort{
    public:
        ServerEventPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > server_function, const std::string& middleware);
        BaseReplyType rx(const BaseRequestType& input);
    private:
        std::function<BaseReplyType (BaseRequestType&) > server_function_;
};

template  <class BaseReplyType, class BaseRequestType>
ClientEventPort<BaseReplyType, BaseRequestType>::ClientEventPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > server_function, const std::string& middleware)
:EventPort(component, port_name, EventPort::Kind::SERVER, middleware){
    server_function_ = server_function;
};

template  <class BaseReplyType, class BaseRequestType>
BaseReplyType ClientEventPort<BaseReplyType, BaseRequestType>::rx(const BaseRequestType& input){
    BaseReplyType return_val;
    if(is_running() && server_function_){
        //Call into the function and log
        logger()->LogComponentEvent(*this, *t, ModelLogger::ComponentEvent::STARTED_FUNC);
        return_val = server_function_(t);
        logger()->LogComponentEvent(*this, *t, ModelLogger::ComponentEvent::FINISHED_FUNC);
    }
    return return_val;
};

#endif //SERVER_EVENT_PORT_HPP
