#ifndef CORE_PORT_EXPORT_H
#define CORE_PORT_EXPORT_H

#include <string>
#include <memory>

#include "../globalinterfaces.hpp"
#include "pubsub/publisherport.hpp"
#include "pubsub/subscriberport.hpp"
#include "requestreply/replierport.hpp"
#include "requestreply/requesterport.hpp"

// REVIEW (Mitch): Unused in non generated code?
// REVIEW (Mitch): Named PortCConstructor intentionally. (The extra C is for C ffi I think)
// REVIEW (Mitch): Should replace with modern type alias
//  using PortCConstructor = Port (*) (const std::string&, std::weak_ptr<Component>);
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
        try{
            const auto& callback_wrapper = component_sp->GetCallback<void, typename PortType::base_type>(port_name);
            return new PortType(component, port_name, callback_wrapper);
        }catch(const std::exception& ex){
            std::cerr << ex.what() << std::endl;
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
        try{
            const auto& callback_wrapper = component_sp->GetCallback<typename PortType::base_reply_type, typename PortType::base_request_type>(port_name);
            return new PortType(component, port_name, callback_wrapper);
        }catch(const std::exception& ex){
            std::cerr << ex.what() << std::endl;
        }
    }
    return nullptr;
};




#endif //CORE_PORT_EXPORT_H