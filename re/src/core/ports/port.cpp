#include "port.h"
#include "../component.h"

Port::Port(std::weak_ptr<Component> component, const std::string& port_name, const Port::Kind& port_kind, const std::string& port_middleware):
Activatable(Class::PORT)
{
    component_ = component;
    set_name(port_name);
    port_kind_ = port_kind;
    port_middleware_ = port_middleware;
}

std::weak_ptr<Component> Port::get_component() const{
    return component_;
};

Port::Kind Port::get_kind() const{
    return port_kind_;
}

std::string Port::get_middleware() const{
    return port_middleware_;
}

void Port::SetKind(const Port::Kind& port_kind){
    port_kind_ = port_kind;
}

uint64_t Port::GetEventsReceived(){
    return received_count_;
}

uint64_t Port::GetEventsProcessed(){
    return processed_count_;
}

uint64_t Port::GetEventsIgnored(){
    return ignored_count_;
}

void Port::EventRecieved(const BaseMessage& message){
    ++received_count_;
}

void Port::EventProcessed(const BaseMessage& message){
    ++processed_count_;
}

void Port::EventIgnored(const BaseMessage& message){
    logger().LogPortUtilizationEvent(*this, message, Logger::UtilizationEvent::IGNORED);
    ++ignored_count_;
}

void Port::ProcessMessageException(const BaseMessage& message, const std::string& error_str){
    logger().LogPortUtilizationEvent(*this, message, Logger::UtilizationEvent::EXCEPTION, error_str);
}
void Port::ProcessGeneralException(const std::string& error_str){
    logger().LogException(*this, error_str);
}

void Port::HandleConfigure(){
    logger().LogLifecycleEvent(*this, Logger::LifeCycleEvent::CONFIGURED);
}

void Port::HandleActivate(){
    logger().LogLifecycleEvent(*this, Logger::LifeCycleEvent::ACTIVATED);
}
void Port::HandlePassivate(){
    logger().LogLifecycleEvent(*this, Logger::LifeCycleEvent::PASSIVATED);
}
void Port::HandleTerminate(){
    logger().LogLifecycleEvent(*this, Logger::LifeCycleEvent::TERMINATED);
}