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
    EXPORT_FUNC Port* ConstructRequestPort(const std::string& port_name, std::weak_ptr<Component> component);
    EXPORT_FUNC Port* ConstructReplyPort(const std::string& port_name, std::weak_ptr<Component> component);
};


template<class PortType, class BaseType>
PortType* ConstructSubscriberPort(const std::string& port_name, std::weak_ptr<Component> component){
    //static_assert(std::is_base_of< ::SubscriberPort<BaseType>, PortType>::value, "PortType must inherit from SubscriberPort");
    //static_assert(std::is_base_of<::BaseMessage, BaseType>::value, "BaseType must inherit from BaseMessage");
    
    std::function<void (BaseType &)> fn;
    
    auto component_sp = component.lock();
	if(component_sp){
		fn = component_sp->GetCallback<BaseType>(port_name);
    }

    return new PortType(component, port_name, fn);
};

template<class T>
T* ConstructPublisherPort(const std::string& port_name, std::weak_ptr<Component> component){
    static_assert(std::is_base_of<Port, T>::value, "T must inherit from Port");
    return new T(component, port_name);
};


#endif //CORE_PORT_EXPORT_H