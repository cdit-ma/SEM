#ifndef CORE_LIBCOMPONENTEXPORTS_H
#define CORE_LIBCOMPONENTEXPORTS_H

#include "globalinterfaces.hpp"
#include "component.h"
#include <string>
#include <memory>
// REVIEW (Mitch): Documentation: This header is used to specify the interface that code-generated
//  out components will expose for construction at model run time

// REVIEW (Mitch): Use "using" type alias here?
//  using ComponentCConstructor = Component (*) (const std::string&);
typedef Component* (ComponentCConstructor) (const std::string&);
extern "C"{
    EXPORT_FUNC Component* ConstructComponent(const std::string& component_name);
};

template<class T>
Component* ConstructComponent(const std::string& component_name){
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
    return new T(component_name);
};




#endif //CORE_LIBPORTEXPORTS_H