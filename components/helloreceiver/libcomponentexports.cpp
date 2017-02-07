#include <core/libcomponentexports.h>
#include <core/component.h>
#include "helloreceiverimpl.h"

Component* ConstructComponent(std::string name)
{
    return new HelloReceiverImpl(name);
};