#ifndef BASE_PORT_PUBLISHER_HPP
#define BASE_PORT_PUBLISHER_HPP

#include "../port.h"
#include "../../modellogger.h"
#include "../../component.h"
#include "../translator.h"

//Interface for a standard templated PublisherPort
template <class BaseType>
class PublisherPort : public Port{
    public:
        PublisherPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware);
        virtual bool Send(const BaseType& t);
};

template <class BaseType>
PublisherPort<BaseType>::PublisherPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
: Port(component, port_name, Port::Kind::PUBLISHER, middleware){

};

template <class BaseType>
bool PublisherPort<BaseType>::Send(const BaseType& message){
    //Log the recieving
    EventRecieved(message);
    
    auto sent_message = is_running();
    
    if(sent_message){
        logger()->LogComponentEvent(*this, message, ModelLogger::ComponentEvent::SENT);
    }
    
    EventProcessed(message, sent_message);
    return sent_message;
};

#endif // BASE_PORT_PUBLISHER_HPP
