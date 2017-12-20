#ifndef CORE_LIBPORTEXPORTS_H
#define CORE_LIBPORTEXPORTS_H

#include "globalinterfaces.hpp"

#include "eventports/eventport.h"
#include "component.h"
#include <string>
#include <memory>


typedef EventPort* (EventPortCConstructor) (const std::string& port_name, std::weak_ptr<Component>);

extern "C"{
    EXPORT_FUNC EventPort* ConstructOutEventPort(const std::string& port_name, std::weak_ptr<Component> component);
    EXPORT_FUNC EventPort* ConstructInEventPort(const std::string& port_name, std::weak_ptr<Component> component);
};

template<class T>
T* ConstructInEventPort(const std::string& port_name, std::weak_ptr<Component> component){
    static_assert(std::is_base_of<EventPort, T>::value, "T must inherit from EventPort");

    std::function<void(::BaseMessage&)> fn;
    
    auto component_sp = component.lock();
	if(component_sp){
		fn = component_sp->GetCallback(port_name);
    }
    return new T(component, port_name, fn);
};

template<class T>
T* ConstructOutEventPort(const std::string& port_name, std::weak_ptr<Component> component){
    static_assert(std::is_base_of<EventPort, T>::value, "T must inherit from EventPort");
    return new T(component, port_name);
};


#endif //CORE_LIBPORTEXPORTS_H