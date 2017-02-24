#ifndef CORE_LIBCOMPONENTEXPORTS_H
#define CORE_LIBCOMPONENTEXPORTS_H

#include "component.h"
#include <string>

#ifdef _WIN32
    #define WIN_DLL_EXPORT __declspec(dllexport)
#else
    #define WIN_DLL_EXPORT
#endif

extern "C"{
    WIN_DLL_EXPORT Component* ConstructComponent(std::string component_name);
}

#endif //CORE_LIBPORTEXPORTS_H