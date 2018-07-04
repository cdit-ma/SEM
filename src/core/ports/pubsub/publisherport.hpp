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
        virtual void Send(const BaseType& message) = 0;
        using base_type = BaseType;
};

template <class BaseType>
PublisherPort<BaseType>::PublisherPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
: Port(component, port_name, Port::Kind::PUBLISHER, middleware){

};


#endif // BASE_PORT_PUBLISHER_HPP
