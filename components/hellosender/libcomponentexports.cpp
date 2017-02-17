#include <core/libcomponentexports.h>
#include <core/component.h>
#include "hellosenderimpl.h"

Component* ConstructComponent(std::string name)
{
    return new HelloSenderImpl(name);
};