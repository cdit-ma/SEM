#include "activatable.h"
#include <iostream>
#include <typeinfo>

Activatable::Activatable(Class c){
    class_ = c;
    logger_ = std::unique_ptr<LoggerProxy>(new LoggerProxy);

    //Bind in Transition functions
    state_machine_.RegisterTransitionFunction(StateMachine::Transition::CONFIGURE, [=](){HandleConfigure();});
    state_machine_.RegisterTransitionFunction(StateMachine::Transition::ACTIVATE,  [=](){HandleActivate();});
    state_machine_.RegisterTransitionFunction(StateMachine::Transition::PASSIVATE, [=](){HandlePassivate();});
    state_machine_.RegisterTransitionFunction(StateMachine::Transition::TERMINATE, [=](){HandleTerminate();});
}

std::string Activatable::get_name() const{
    return name_;
}

Activatable::Class Activatable::get_class() const{
    return class_;
}

void Activatable::set_name(std::string name){
    name_ = name;
}

std::string Activatable::get_id() const{
    return id_;
}

void Activatable::set_id(std::string id){
    id_ = id;
}

std::string Activatable::get_type() const{
    return type_;
}

void Activatable::set_type(std::string type){
    type_ = type;
}

bool Activatable::Activate(){
    return state_machine_.Activate();
}

bool Activatable::Configure(){
    return state_machine_.Configure();
}

bool Activatable::Passivate(){
    return state_machine_.Passivate();
}

bool Activatable::Terminate(){
    return state_machine_.Terminate();
}

StateMachine::State Activatable::get_state(){
    return state_machine_.get_state();
}

LoggerProxy& Activatable::logger() const{
    return *logger_;
};

bool Activatable::process_event(){
    if(state_machine_.get_state() == StateMachine::State::RUNNING){
        return state_machine_.get_transition() == StateMachine::Transition::NO_TRANSITION;
    }
    return false;
}

std::weak_ptr<Attribute> Activatable::AddAttribute(std::unique_ptr<Attribute> attribute){
    std::lock_guard<std::mutex> lock(attributes_mutex_);
    if(attribute){
        std::string name = attribute->get_name();
        if(attributes_.count(name) == 0){
            attributes_[name] = std::move(attribute);
            return attributes_[name];
        }else{
            logger().LogException(*this, std::string(GET_FUNC) + " " + get_name() + ": Already got an attribute with name: '" + name + "'");
        }
    }else{
        logger().LogException(*this, std::string(GET_FUNC) + " " + get_name() + ": Cannot attach a NULL attribute");
    }
    return std::weak_ptr<Attribute>();
}

std::weak_ptr<Attribute> Activatable::ConstructAttribute(const ATTRIBUTE_TYPE type, const std::string name){
    //Try get the attribute, if it exists
    auto attribute_sp = GetAttribute(name).lock();
    if(attribute_sp){
        if(attribute_sp->get_type() != type){
            logger().LogException(*this, std::string(GET_FUNC) + " " + get_name() + ": Already got an attached Attribute with name '" + name + "' with a differing type");
            attribute_sp.reset();
        }
    }else{
        attribute_sp = AddAttribute(std::unique_ptr<Attribute>(new Attribute(type, name))).lock();
    }
    return attribute_sp;
}

std::weak_ptr<Attribute> Activatable::GetAttribute(const std::string& name){
    std::lock_guard<std::mutex> lock(attributes_mutex_);
    if(attributes_.count(name)){
        return attributes_[name];
    }else{
        return std::weak_ptr<Attribute>();
    }
}

std::shared_ptr<Attribute> Activatable::GetAttribute(const std::string& name, const ATTRIBUTE_TYPE type){
    auto attr = GetAttribute(name).lock();
    if(attr){
        if(attr->get_type() != type){
            attr.reset();
        }
    }
    return attr;
}