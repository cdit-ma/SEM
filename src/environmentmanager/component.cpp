#include "component.h"
#include "node.h"
#include "port.h"

using namespace EnvironmentManager;

Component::Component(const Node& node, const std::string& id, const std::string& name) : parent_(node){
    id_ = id;
    name_ = name;
}
std::string Component::GetId(){
    return id_;
};
std::string Component::GetName(){
    return name_;
};
Node& Component::GetParent(){
    return parent_;
}

void Component::AddPort(std::unique_ptr<Port> port){
    ports_.insert({port.GetId(), std::move(port)});
}

bool SetDirty(){
    parent_.SetDirty();
    dirty_ = true;
}

bool IsDirty(){
    return dirty_;
}