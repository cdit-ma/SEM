#include <core/libportexport.h>

// Include the convert function
#include "proto/message/convert.h"

// Include the zmq specific templated classes
#include <middleware/zmq/ineventport.hpp>
#include <middleware/zmq/outeventport.hpp>

EventPort* ConstructInEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	return ConstructInEventPort<zmq::InEventPort<Base::Message, Message> >(port_name, component);
};

EventPort* ConstructOutEventPort(const std::string& port_name, std::weak_ptr<Component> component){
	return ConstructOutEventPort<zmq::OutEventPort<Base::Message, Message> >(port_name, component);
};
