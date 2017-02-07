#ifndef CORE_LIBPORTEXPORTS_H
#define CORE_LIBPORTEXPORTS_H

#include "eventport.h"
#include "component.h"
#include <string>

extern "C"{
    EventPort* ConstructRx(std::string port_name, Component* component);
    EventPort* ConstructTx(std::string port_name, Component* component);
}

#endif //CORE_LIBPORTEXPORTS_H