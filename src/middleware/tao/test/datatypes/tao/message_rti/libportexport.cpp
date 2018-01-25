#include <core/libportexport.h>

// Include the convert function
#include "convert.h"

// Including 'rti' generated header
#include "message.hpp"

// Include the rti specific templated classes
#include <middleware/rti/ineventport.hpp>
#include <middleware/rti/outeventport.hpp>

EventPort* ConstructInEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	return ConstructInEventPort<rti::InEventPort<Base::Message, Message> >(port_name, component);
};

EventPort* ConstructOutEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	return ConstructOutEventPort<rti::OutEventPort<Base::Message, Message> >(port_name, component);
};
