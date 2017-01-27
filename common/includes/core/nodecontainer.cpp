#include "nodecontainer.h"
#include <iostream>
#include "translate.h"
#include "periodiceventport.h"
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


::EventPort* NodeContainer::construct_periodic_event(Component* component, std::string port_name){
    return new PeriodicEventPort(component, port_name, component->get_callback(port_name), 1000);
}

void NodeContainer::configure(NodeManager::ControlMessage* message){
    auto n = message->mutable_node();
    for(auto c : n->components()){
        auto component = get_component(c.name());

        if(!component){
            //std::cout << "Can't find Component: '" << c.name() << "' Constructing!" << std::endl;
            //Construct Component
            component = construct_component(c.type(), c.name());
        }

        if(component){
            for(auto a: c.attributes()){
                auto attribute = component->get_attribute(a.name());
                if(attribute){
                    std::cout << c.name() << " Setting Attribute: " << a.name() <<  std::endl;
                    set_attribute_from_pb(&a, attribute);
                }
            }
            for(auto p : c.ports()){
                auto port = component->get_event_port(p.name());

                if(!port){
                    //std::cout << "Can't find EventPort: '" << p.name() << "' Constructing!" << std::endl;
                    std::string middleware = "zmq";
                    switch(p.type()){
                        case NodeManager::EventPort::IN:{
                            port = construct_rx(middleware, p.message_type(), component, p.name());
                            break;
                        }
                        case NodeManager::EventPort::OUT:{
                            port = construct_tx(middleware, p.message_type(), component, p.name());
                            break;
                        }
                        case NodeManager::EventPort::PERIODIC:{
                            port = construct_periodic_event(component, p.name());
                            break;
                        }
                        default:
                            break;
                    }
                }
                if(port){
                    std::map<std::string, ::Attribute*> attributes_;

                    for(auto a: p.attributes()){
                        auto att = set_attribute_from_pb(&a);
                        if(att){
                            attributes_[att->name] = att;
                        }
                    }

                    port->startup(attributes_);
                }
            }
            
        }
    }
}

bool NodeContainer::activate_all(){
    for(auto c : components_){
        std::cout << "NodeContainer::activate_all() Component:" << c.second->get_name() << std::endl;
        c.second->activate();
    }
    return true;
}

bool NodeContainer::passivate_all(){
    for(auto c : components_){
        std::cout << "NodeContainer::passivate_all() Component:" << c.second << std::endl;
        c.second->passivate();
    }

    return true;
}
void NodeContainer::teardown(){

    passivate_all();
    for(auto it=components_.begin(); it!=components_.end();){
        delete it->second;
        it = components_.erase(it);
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
        //std::cout << "Can't Find Component: " << component_name  << std::endl;
        return 0;
    }else{
        return search->second;
    }
}