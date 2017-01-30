#include "nodecontainer.h"
#include <iostream>
#include <algorithm>
#include <string>
#include "translate.h"
#include "periodiceventport.h"
#include "controlmessage.pb.h"

#include <dlfcn.h>

//Converts std::string to lower
std::string to_lower(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}


NodeContainer::NodeContainer(std::string library_path){
    library_path_ = library_path;
}

NodeContainer::~NodeContainer(){
    teardown();

    for(auto it=loaded_libraries_.begin(); it!=loaded_libraries_.end();){
        auto lib_handle = it->second;
        if(dlclose(lib_handle)){
            std::cout << "DLL Closed: " << it->first << std::endl;
        }
        it = loaded_libraries_.erase(it);
    }
}

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

EventPort* NodeContainer::construct_periodic_event(Component* component, std::string port_name){
    return new PeriodicEventPort(component, port_name, component->get_callback(port_name), 1000);
}

void NodeContainer::configure(NodeManager::ControlMessage* message){
    std::cout << "CONFIGURE" << std::endl;
    auto n = message->mutable_node();
    for(auto c : n->components()){
        auto component = get_component(c.name());

        if(!component){
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

                std::string middleware;
                std::cout << p.name() << " MW: " << NodeManager::EventPort_Middleware_Name(p.middleware()) << std::endl;
                switch(p.middleware()){
                    case NodeManager::EventPort_Middleware_ZMQ:{
                        middleware = "zmq";
                        break;
                    }
                    case NodeManager::EventPort_Middleware_RTI_DDS:{
                        middleware = "rti";
                        break;
                    }
                    case NodeManager::EventPort_Middleware_OSPL_DDS:{
                        middleware = "ospl";
                        break;
                    }
                    case NodeManager::EventPort_Middleware_QPID:{
                        middleware = "qpid";
                        break;
                    }
                    default:
                        std::cout << "NO MIDDLEWARE!" << std::endl;
                        break;
                }


                if(!port){
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

                //Configure the port
                if(port){
                    std::map<std::string, ::Attribute*> attributes_;

                    for(auto a: p.attributes()){
                        auto att = set_attribute_from_pb(&a);
                        if(att){
                            attributes_[att->name] = att;
                        }
                    }
                    //Configure the port
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
        delete c;
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

void* NodeContainer::load_library(std::string library_path){
    //If we haven't seen the library_path before, try and load it.
    if(!loaded_libraries_.count(library_path)){
        //Get a handle to the dynamically linked library
        void* lib_handle = dlopen(library_path.c_str(), RTLD_LAZY);
        
        //Check for errors
        char* error = dlerror();
        if(error){
            std::cerr << "DLL Error: " << error << std::endl;
        }else{
            //Add it to the map of loaded libraries
            loaded_libraries_[library_path] = lib_handle;
        }
    }

    //Get the loaded library
    if(loaded_libraries_.count(library_path)){
        //Get the already loaded library
        return loaded_libraries_[library_path];
    }
    return 0;
}

void* NodeContainer::get_library_function(std::string library_path, std::string function_name){
    void* lib_handle = load_library(library_path);
    return get_library_function(lib_handle, function_name);
}

void* NodeContainer::get_library_function(void* lib_handle, std::string function_name){
    if(lib_handle){
        char* error = dlerror();
        void* function = dlsym(lib_handle, function_name.c_str());
        error = dlerror();
        if(function && !error){
            return function;
        }else{
            std::cerr << "DLL Error Linking '" + function_name + "': " << error << std::endl;
        }
    }
    return 0;
}



EventPort* NodeContainer::construct_tx(std::string middleware, std::string datatype, Component* component, std::string port_name){
    if(!tx_constructors_.count(middleware)){
        auto lib_path = library_path_ + "/lib" + to_lower(middleware) + "_ports.so";

        //Get the function
        void* function = get_library_function(lib_path, "construct_tx");
        if(function){
            //Cast as EventPort* construct_tx(std::string, Component*, std::string)
            auto typed_function = (EventPort* (*) (std::string, Component*, std::string)) function;
            //Add to the lookup
            tx_constructors_[middleware] = typed_function;
        }
    }

    if(tx_constructors_.count(middleware)){
        return tx_constructors_[middleware](datatype, component, port_name);
    }
    return 0;
}

EventPort* NodeContainer::construct_rx(std::string middleware, std::string datatype, Component* component, std::string port_name){
    if(!rx_constructors_.count(middleware)){
        auto lib_path = library_path_ + "/lib" + to_lower(middleware) + "_ports.so";

        //Get the function
        void* function = get_library_function(lib_path, "construct_rx");
        if(function){
            //Cast as EventPort* construct_rx(std::string, Component*, std::string)
            auto typed_function = (EventPort* (*) (std::string, Component*, std::string)) function;
            //Add to the lookup
            rx_constructors_[middleware] = typed_function;
        }
    }

    if(rx_constructors_.count(middleware)){
        return rx_constructors_[middleware](datatype, component, port_name);
    }
    return 0;
}

Component* NodeContainer::construct_component(std::string component_type, std::string component_name){
    if(!component_constructors_.count(component_type)){
        auto lib_path = library_path_ + "/libcomponents_" + to_lower(component_type) + ".so";

        //Get the function
        void* function = get_library_function(lib_path, "construct_component");
        if(function){
            //Cast as Component* construct_component(std::string)
            auto typed_function = (Component* (*) (std::string)) function;
            //Add to the lookup
            component_constructors_[component_type] = typed_function;
        }
    }

    if(component_constructors_.count(component_type)){
        return component_constructors_[component_type](component_name);
    }
    return 0;
}

void NodeContainer::add_tx_constructor(std::string middleware, TxConstructor constructor){
    if(!tx_constructors_.count(middleware)){
        tx_constructors_[middleware] = constructor;
    }
}
void NodeContainer::add_rx_constructor(std::string middleware, TxConstructor constructor){
    if(!rx_constructors_.count(middleware)){
        rx_constructors_[middleware] = constructor;
    }
}

void NodeContainer:: add_component_constructor(std::string component_type, ComponentConstructor constructor){
    if(!component_constructors_.count(component_type)){
        component_constructors_[component_type] = constructor;
    }
}


