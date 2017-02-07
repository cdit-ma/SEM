#include <core/libcomponentexports.h>
#include <core/component.h>
#include "helloreceiverimpl.h"

Component* construct_component(std::string name)
{
    return new HelloReceiverImpl(name);
};