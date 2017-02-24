#ifndef CORE_LIBPORTEXPORTS_H
#define CORE_LIBPORTEXPORTS_H

#include "eventport.h"
#include "component.h"
#include <string>

#ifdef _WIN32
    #define WIN_DLL_EXPORT __declspec(dllexport)
#else
    #define WIN_DLL_EXPORT
#endif


extern "C"{
    WIN_DLL_EXPORT EventPort* ConstructRx(std::string port_name, Component* component);
    WIN_DLL_EXPORT EventPort* ConstructTx(std::string port_name, Component* component);
}

#endif //CORE_LIBPORTEXPORTS_H