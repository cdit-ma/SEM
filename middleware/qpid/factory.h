#ifndef QPID_FACTORY_H
#define QPID_FACTORY_H

#include <core/component.h>
#include <core/eventport.h>
#include <core/basemessage.h>

namespace qpid{
    EventPort* construct_rx(std::string type, Component* component, std::string port_name);
    EventPort* construct_tx(std::string type, Component* component, std::string port_name);
};
#endif //QPID_FACTORY_H