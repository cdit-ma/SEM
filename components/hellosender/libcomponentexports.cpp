#include <core/libcomponentexports.h>
#include <core/component.h>
#include "hellosenderimpl.h"

Component* construct_component(std::string name)
{
    return new HelloSenderImpl(name);
};