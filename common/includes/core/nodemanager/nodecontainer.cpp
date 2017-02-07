#include "nodecontainer.h"
#include <iostream>
#include <algorithm>
#include <string>

#include <dlfcn.h>

#include "../translate.h"
#include "../periodiceventport.h"
#include "controlmessage.pb.h"



//Converts std::string to lower
std::string to_lower(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}


NodeContainer::NodeContainer(std::string library_path){
    library_path_ = library_path;
}

NodeContainer::~NodeContainer(){
    Teardown();

    for(auto it=loaded_libraries_.begin(); it!=loaded_libraries_.end();){
        auto lib_handle = it->second;
        if(dlclose(lib_handle)){
            std::cout << "DLL Closed: " << it->first << std::endl;
        }
        it = loaded_libraries_.erase(it);
    }
}

bool NodeContainer::Activate(std::string component_name){
    Component* component = GetComponent(component_name);
    if(component){
        return component->Activate();
    }
    return false;
}
bool NodeContainer::Passivate(std::string component_name){
    Component* component = GetComponent(component_name);
    if(component){
        return component->Passivate();
    }
    return false;
}

EventPort* NodeContainer::ConstructPeriodicEvent(Component* component, std::string port_name){
    return new PeriodicEventPort(component, port_name, component->GetCallback(port_name), 1000);
}

void NodeContainer::Configure(NodeManager::ControlMessage* message){
    std::cout << "CONFIGURE" << std::endl;
    auto n = message->mutable_node();
    for(auto c : n->components()){
        auto component = GetComponent(c.name());

        if(!component){
            //Construct Component
            component = ConstructComponent(c.type(), c.name());
        }

        if(component){
            for(auto a: c.attributes()){
                auto attribute = component->GetAttribute(a.name());
                if(attribute){
                    std::cout << c.name() << " Setting Attribute: " << a.name() <<  std::endl;
                    SetAttributeFromPb(&a, attribute);
                }
            }
            for(auto p : c.ports()){
                auto port = component->GetEventPort(p.name());

                //Get the middleware
                std::string middleware = NodeManager::EventPort_Middleware_Name(p.middleware());
                std::cout << "Middleware: " << middleware << std::endl;

                if(!port){
                    switch(p.type()){
                        case NodeManager::EventPort::IN:{
                            port = ConstructRx(middleware, p.message_type(), component, p.name());
                            break;
                        }
                        case NodeManager::EventPort::OUT:{
                            port = ConstructTx(middleware, p.message_type(), component, p.name());
                            break;
                        }
                        case NodeManager::EventPort::PERIODIC:{
                            port = ConstructPeriodicEvent(component, p.name());
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
                        auto att = SetAttributeFromPb(&a);
                        if(att){
                            attributes_[att->name] = att;
                        }
                    }
                    //Configure the port
                    port->Startup(attributes_);
                }
            }
        }
    }
}

bool NodeContainer::ActivateAll(){
    for(auto c : components_){
        std::cout << "NodeContainer::ActivateAll() Component:" << c.second->get_name() << std::endl;
        c.second->Activate();
    }
    return true;
}

bool NodeContainer::PassivateAll(){
    for(auto c : components_){
        std::cout << "NodeContainer::PassivateAll() Component:" << c.second << std::endl;
        c.second->Passivate();
    }

    return true;
}
void NodeContainer::Teardown(){
    PassivateAll();
    for(auto it=components_.begin(); it!=components_.end();){
        auto c = it->second;
        delete c;
        it = components_.erase(it);
    }
}

bool NodeContainer::AddComponent(Component* component){
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

Component* NodeContainer::GetComponent(std::string component_name){
    //Search pub_lookup_ for key
    auto search = components_.find(component_name);
    
    if(search == components_.end()){
        //std::cout << "Can't Find Component: " << component_name  << std::endl;
        return 0;
    }else{
        return search->second;
    }
}

void* NodeContainer::LoadLibrary(std::string library_path){
    //If we haven't seen the library_path before, try and load it.
    if(!loaded_libraries_.count(library_path)){
        auto start = std::chrono::system_clock::now();
        //Get a handle to the dynamically linked library
        void* lib_handle = dlopen(library_path.c_str(), RTLD_LAZY);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end-start;
        std::cout << "dlopen: " << library_path <<  " " << (diff).count() << " μs" << std::endl;
        
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

void* NodeContainer::GetLibraryFunction(std::string library_path, std::string function_name){
    void* lib_handle = LoadLibrary(library_path);
    return GetLibraryFunction(lib_handle, function_name);
}

void* NodeContainer::GetLibraryFunction(void* lib_handle, std::string function_name){
    if(lib_handle){
        char* error = dlerror();
        auto start = std::chrono::system_clock::now();
        void* function = dlsym(lib_handle, function_name.c_str());
        auto end = std::chrono::system_clock::now();
        
        std::cout << "dlsym: " << function_name << (end - start).count() << " μs" << std::endl;
        
        error = dlerror();
        if(function && !error){
            return function;
        }else{
            std::cerr << "DLL Error Linking '" + function_name + "': " << error << std::endl;
        }
    }
    return 0;
}



EventPort* NodeContainer::ConstructTx(std::string middleware, std::string datatype, Component* component, std::string port_name){
    auto p = to_lower(middleware + "_" + datatype);
    std::cout << p << std::endl;
    if(!tx_constructors_.count(p)){
        //auto lib_path = library_path_ + "/libports_" + to_lower(middleware) + ".so";
        auto lib_path = library_path_ + "/lib" + p + ".so";

        //Get the function
        void* function = GetLibraryFunction(lib_path, "ConstructTx");
        if(function){
            //Cast as EventPort* ConstructRx(std::string, std::string, Component*)
            auto typed_function = (EventPort* (*) (std::string, Component*)) function;
            //Add to the lookup
            tx_constructors_[p] = typed_function;
        }
    }

    if(tx_constructors_.count(p)){
        return tx_constructors_[p](port_name, component);
    }
    return 0;
}

EventPort* NodeContainer::ConstructRx(std::string middleware, std::string datatype, Component* component, std::string port_name){
    auto p = to_lower(middleware + "_" + datatype);
    std::cout << p << std::endl;
    if(!rx_constructors_.count(p)){
        //auto lib_path = library_path_ + "/libports_" + to_lower(middleware) + ".so";
        auto lib_path = library_path_ + "/lib" + p + ".so";

        //Get the function
        void* function = GetLibraryFunction(lib_path, "ConstructRx");
        if(function){
            //Cast as EventPort* ConstructRx(std::string, std::string, Component*)
            auto typed_function = (EventPort* (*) (std::string, Component*)) function;
            //Add to the lookup
            rx_constructors_[p] = typed_function;
        }
    }

    if(rx_constructors_.count(p)){
        return rx_constructors_[p](port_name, component);
    }
    return 0;
}

Component* NodeContainer::ConstructComponent(std::string component_type, std::string component_name){
    Component* c = 0;
    if(!component_constructors_.count(component_type)){
        auto lib_path = library_path_ + "/libcomponents_" + to_lower(component_type) + ".so";

        //Get the function
        void* function = GetLibraryFunction(lib_path, "ConstructComponent");
        if(function){
            //Cast as Component* ConstructComponent(std::string)
            auto typed_function = (Component* (*) (std::string)) function;
            //Add to the lookup
            component_constructors_[component_type] = typed_function;
        
        }
    }

    if(component_constructors_.count(component_type)){
        c = component_constructors_[component_type](component_name);
        if(c){
            //Add the Component to the NodeContainer
            AddComponent(c);
        }
    }
    return c;
}

void NodeContainer::AddTxConstructor(std::string middleware, TxConstructor constructor){
    if(!tx_constructors_.count(middleware)){
        tx_constructors_[middleware] = constructor;
    }
}
void NodeContainer::AddRxConstructor(std::string middleware, TxConstructor constructor){
    if(!rx_constructors_.count(middleware)){
        rx_constructors_[middleware] = constructor;
    }
}

void NodeContainer:: AddComponentConstructor(std::string component_type, ComponentConstructor constructor){
    if(!component_constructors_.count(component_type)){
        component_constructors_[component_type] = constructor;
    }
}


