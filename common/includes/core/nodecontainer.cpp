#include "nodecontainer.h"
#include <iostream>
#include <algorithm>
#include <string>
#include "translate.h"
#include "periodiceventport.h"
#include "controlmessage.pb.h"

#include <dlfcn.h>

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

void NodeContainer::set_library_path(std::string library_path){
    library_path_ = library_path;
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
        auto c = it->second;

        //c->destructor_fn(c);
        //delete it->second;
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

void* load_library(std::string library_path){

    std::cout << "Trying Load DLL: " << library_path << std::endl;

    char* error;
    //Get a handle to the dynamically linked library
    void * library_handle = dlopen(library_path.c_str(), RTLD_LAZY);
        
    //Check for errors
    error = dlerror();
    if(error){
        std::cerr << "DLL Error: " << error << std::endl;
        return 0;
    }
    
    return library_handle;
}

std::string to_lower(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}



EventPort* NodeContainer::construct_tx(std::string middleware, std::string datatype, Component* component, std::string port_name){
    std::cout << "construct_tx(" << middleware << ", " << datatype << ", " << port_name << "): " << std::endl;

    if(!TxConstructors_.count(middleware)){
        //Try Load
        auto lib = load_library(library_path_ + "/lib" + to_lower(middleware) + "_ports.so");

        if(lib){
            std::function<EventPort* (std::string, Component*, std::string)> fn;
            
            fn = (EventPort* (*) (std::string, Component*, std::string)) dlsym(lib, "construct_tx");
            char * error;
            error = dlerror();
            if(fn && !error){
                TxConstructors_[middleware] = fn;
            }else{
                std::cerr << "DLL Error Linking 'construct_tx': " << error << std::endl;
            }
        }
    }
    if(TxConstructors_.count(middleware)){
        return TxConstructors_[middleware](datatype, component, port_name);
    }

    std::cout << "\tNo Constructor" << std::endl;
    return 0;
}

EventPort* NodeContainer::construct_rx(std::string middleware, std::string datatype, Component* component, std::string port_name){
    std::cout << "construct_rx(" << middleware << ", " << datatype << ", " << port_name << "): " << std::endl;
    if(!RxConstructors_.count(middleware)){
        //Try Load
        auto lib = load_library(library_path_ + "/lib" + to_lower(middleware) + "_ports.so");

        if(lib){
            std::function<EventPort* (std::string, Component*, std::string)> fn;
            
            fn  = (EventPort* (*) (std::string, Component*, std::string)) dlsym(lib, "construct_rx");
            char * error;
            error = dlerror();
            if(fn && !error){
                RxConstructors_[middleware] = fn;
            }else{
                std::cerr << "DLL Error Linking 'construct_rx': " << error << std::endl;
            }
        }
    }

    if(RxConstructors_.count(middleware)){
        return RxConstructors_[middleware](datatype, component, port_name);
    }
    std::cout << "\tNo Constructor" << std::endl;
    return 0;
}

Component* NodeContainer::construct_component(std::string component_type, std::string component_name){
    std::cout << "construct_component(" << component_type << ", " << component_name << "): " << std::endl;
    
    if(!ComponentConstructors_.count(component_type)){
        //Try Load
        auto lib = load_library(library_path_ + "/libcomponents_" + to_lower(component_type) + ".so");

        if(lib){
            std::function<Component* (std::string)> fn;
            fn = (Component* (*) (std::string)) dlsym(lib, "construct_component");
            char* error;
            error = dlerror();
            if(fn && !error){
                ComponentConstructors_[component_type] = fn;
            }else{
                std::cerr << "DLL Error Linking 'construct_component': " << error << std::endl;
            }
        }
    }

    Component* c = 0;
    if(ComponentConstructors_.count(component_type)){
        c = ComponentConstructors_[component_type](component_name);
    }
    if(c){
        add_component(c);
    }else{
        std::cout << "\tNo Constructor" << std::endl;
    }
    return c;
}

void NodeContainer::add_tx_constructor(std::string middleware, TxConstructor constructor){
    if(!TxConstructors_.count(middleware)){
        TxConstructors_[middleware] = constructor;
    }
}
void NodeContainer::add_rx_constructor(std::string middleware, TxConstructor constructor){
    if(!RxConstructors_.count(middleware)){
        RxConstructors_[middleware] = constructor;
    }
}

void NodeContainer:: add_component_constructor(std::string component_type, ComponentConstructor constructor){
    if(!ComponentConstructors_.count(component_type)){
        ComponentConstructors_[component_type] = constructor;
    }
}


