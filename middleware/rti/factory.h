#ifndef RTI_FACTORY_H
#define RTI_FACTORY_H

#include <core/component.h>
#include <core/eventport.h>
#include <core/basemessage.h>

namespace rti{
    EventPort* construct_rx(std::string type, Component* component, std::string port_name);
    EventPort* construct_tx(std::string type, Component* component, std::string port_name);
};
#endif //RTI_FACTORY_H