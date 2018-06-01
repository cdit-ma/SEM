#ifndef CLIENT_EVENT_PORT_HPP
#define CLIENT_EVENT_PORT_HPP

#include "../eventport.h"

template <class BaseReplyType, class BaseRequestType> class ClientEventPort : public EventPort{
    public:
        ClientEventPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware);

        virtual BaseReplyType tx(const BaseRequestType& type) = 0;
};

template  <class BaseReplyType, class BaseRequestType>
ClientEventPort<BaseReplyType, BaseRequestType>::ClientEventPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
:EventPort(component, port_name, EventPort::Kind::CLIENT, middleware){

};

#endif //CLIENT_EVENT_PORT_HPP
