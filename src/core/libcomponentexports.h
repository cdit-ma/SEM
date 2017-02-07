#ifndef CORE_LIBCOMPONENTEXPORTS_H
#define CORE_LIBCOMPONENTEXPORTS_H

#include "component.h"
#include <string>

extern "C"{
    Component* ConstructComponent(std::string component_name);
}

#endif //CORE_LIBPORTEXPORTS_H