#include "nodecontainer.h"
#include <iostream>

#include "controlmessage.pb.h"

bool NodeContainer::activate(std::string component_name){
    Component* component = get_component(component_name);
    if(component){
        return component->activate();
    }
    return false;
}
bool NodeContainer::passivate(std::string component_name){
    Component* component = get_component(component_name);
    if(component){
        return component->passivate();
    }
    return false;
}

void NodeContainer::configure(NodeManager::ControlMessage* message){

    auto n = message->mutable_node();

    std::cout << "WINRAR: " << message->DebugString() << std::endl;
    for(auto c : n->components()){
        std::cout << c.name() << std::endl;
        
    }
    
}

bool NodeContainer::activate_all(){
    for(auto c : components_){
        std::cout << "Activating: " << c.second << std::endl;
        c.second->activate();
    }
    return true;
}

bool NodeContainer::passivate_all(){
    for(auto c : components_){
        std::cout << "Passivating: " << c.second << std::endl;
        c.second->passivate();
    }
    return true;
}
void NodeContainer::teardown(){
    for (auto c : components_) {
        components_.erase(c.first);
    }
}

bool NodeContainer::add_component(Component* component){
    std::string component_name = component->get_name();

    //Search pub_lookup_ for key
    auto search = components_.find(component_name);
    
    if(search == components_.end()){
        std::pair<std::string, Component*> insert_pair(component_name, component);
        //Insert into hash
        components_.insert(insert_pair);
        return true;
    }else{
        std::cout << "'" << component_name << "' NOT A UNIQUE NAME!" << std::endl;
        return false;
    }
}

Component* NodeContainer::get_component(std::string component_name){
    //Search pub_lookup_ for key
    auto search = components_.find(component_name);
    
    if(search == components_.end()){
        return 0;
    }else{
        return search->second;
    }
}