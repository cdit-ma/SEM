#ifndef BASE_PORT_REPLIER_HPP
#define BASE_PORT_REPLIER_HPP

#include "../port.h"
#include "../../modellogger.h"
#include "../../component.h"

template <class BaseReplyType, class BaseRequestType>
class ReplierPort : public Port{
    public:
        ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > callback_function, const std::string& middleware);
    protected:
        BaseReplyType ProcessRequest(BaseRequestType& type);
    private:
        std::function<BaseReplyType (BaseRequestType&) > callback_function_;
};

template  <class BaseReplyType, class BaseRequestType>
ReplierPort<BaseReplyType, BaseRequestType>::ReplierPort(std::weak_ptr<Component> component, const std::string& port_name, std::function<BaseReplyType (BaseRequestType&) > callback_function, const std::string& middleware)
: Port(component, port_name, Port::Kind::REPLIER, middleware){
    callback_function_ = callback_function;
};

template  <class BaseReplyType, class BaseRequestType>
BaseReplyType ReplierPort<BaseReplyType, BaseRequestType>::ProcessRequest(BaseRequestType& request){
    BaseReplyType return_val;

    auto process_message = is_running() && callback_function_;
    
    if(process_message){
        logger()->LogComponentEvent(*this, request, ModelLogger::ComponentEvent::STARTED_FUNC);
        //Call into the function and log
        return_val = callback_function_(request);
        logger()->LogComponentEvent(*this, request, ModelLogger::ComponentEvent::FINISHED_FUNC);
    }

    EventProcessed(request, process_message);

    return return_val;
};

#endif // BASE_PORT_REPLIER_HPP
