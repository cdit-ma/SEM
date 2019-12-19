#ifndef BASE_PORT_PUBLISHER_HPP
#define BASE_PORT_PUBLISHER_HPP

#include "../port.h"
#include "../../component.h"
#include "../translator.h"

//Interface for a standard templated PublisherPort
template <class BaseType>
class PublisherPort : public Port{
    public:
    // REVIEW (Mitch): This constructor will need to change when/if we rework port.h. Use parent
    // component ID over component ref,
    //  use middleware enum over middleware string.
    PublisherPort(std::weak_ptr<Component> component,
                  const std::string& port_name,
                  const std::string& middleware);
    virtual void Send(const BaseType& message) = 0;
    // REVIEW (Mitch): I believe this type alias is used in code generated code
    using base_type = BaseType;
};

template <class BaseType>
PublisherPort<BaseType>::PublisherPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
: Port(component, port_name, Port::Kind::PUBLISHER, middleware){

};


#endif // BASE_PORT_PUBLISHER_HPP
