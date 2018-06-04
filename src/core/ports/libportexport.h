#ifndef CORE_PORT_EXPORT_H
#define CORE_PORT_EXPORT_H

#include <string>
#include <memory>

#include "../globalinterfaces.hpp"
#include "../component.h"
#include "port.h"


typedef Port* (PortCConstructor) (const std::string& port_name, std::weak_ptr<Component>);

extern "C"{
    EXPORT_FUNC Port* ConstructPublisherPort(const std::string& port_name, std::weak_ptr<Component> component);
    EXPORT_FUNC Port* ConstructSubscriberPort(const std::string& port_name, std::weak_ptr<Component> component);
};

template<class T>
T* ConstructSubscriberPort(const std::string& port_name, std::weak_ptr<Component> component){
    static_assert(std::is_base_of<Port, T>::value, "T must inherit from Port");

    std::function<void(::BaseMessage&)> fn;
    
    auto component_sp = component.lock();
	if(component_sp){
		fn = component_sp->GetCallback(port_name);
    }
    return new T(component, port_name, fn);
};

template<class T>
T* ConstructPublisherPort(const std::string& port_name, std::weak_ptr<Component> component){
    static_assert(std::is_base_of<Port, T>::value, "T must inherit from Port");
    return new T(component, port_name);
};


#endif //CORE_PORT_EXPORT_H