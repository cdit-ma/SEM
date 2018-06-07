#ifndef CORE_PORT_EXPORT_H
#define CORE_PORT_EXPORT_H

#include <string>
#include <memory>

#include "../globalinterfaces.hpp"
#include "pubsub/publisherport.hpp"
#include "pubsub/subscriberport.hpp"
#include "requestreply/replierport.hpp"
#include "requestreply/requesterport.hpp"


typedef Port* (PortCConstructor) (const std::string& port_name, std::weak_ptr<Component>);

extern "C"{
    EXPORT_FUNC Port* ConstructPublisherPort(const std::string& port_name, std::weak_ptr<Component> component);
    EXPORT_FUNC Port* ConstructSubscriberPort(const std::string& port_name, std::weak_ptr<Component> component);
    EXPORT_FUNC Port* ConstructRequesterPort(const std::string& port_name, std::weak_ptr<Component> component);
    EXPORT_FUNC Port* ConstructReplierPort(const std::string& port_name, std::weak_ptr<Component> component);
};

template<class PortType>
PortType* ConstructSubscriberPort(const std::string& port_name, std::weak_ptr<Component> component){
    static_assert(std::is_base_of<SubscriberPort<typename PortType::base_type>, PortType>::value, "PortType must inherit from SubscriberPort");
    auto component_sp = component.lock();
	if(component_sp){
        auto callback_wrapper = component_sp->GetCallback<void, typename PortType::base_type>(port_name);
        if(callback_wrapper){
            return new PortType(component, port_name, callback_wrapper->callback_fn);
        }
    }
    return nullptr;
};

template<class PortType>
PortType* ConstructPublisherPort(const std::string& port_name, std::weak_ptr<Component> component){
    static_assert(std::is_base_of<PublisherPort<typename PortType::base_type>, PortType>::value, "PortType must inherit from PublisherPort");
    return new PortType(component, port_name);
};

template<class PortType>
PortType* ConstructRequesterPort(const std::string& port_name, std::weak_ptr<Component> component){
    static_assert(std::is_base_of<RequesterPort<typename PortType::base_reply_type, typename PortType::base_request_type>, PortType>::value, "PortType must inherit from RequesterPort");
    return new PortType(component, port_name);
};

template<class PortType>
PortType* ConstructReplierPort(const std::string& port_name, std::weak_ptr<Component> component){
    static_assert(std::is_base_of<ReplierPort<typename PortType::base_reply_type, typename PortType::base_request_type>, PortType>::value, "PortType must inherit from ReplierPort");
    auto component_sp = component.lock();
	if(component_sp){
        auto callback_wrapper = component_sp->GetCallback<typename PortType::base_reply_type, typename PortType::base_request_type>(port_name);
        if(callback_wrapper){
            return new PortType(component, port_name, callback_wrapper->callback_fn);
        }
    }
    return nullptr;
};




#endif //CORE_PORT_EXPORT_H