#include "nodecontainer.h"
#include <iostream>
#include <algorithm>
#include <string>


#ifdef _WIN32
    #include <windows.h>
    #include <stdio.h>
#else
    #include <dlfcn.h>
#endif


#include "../periodiceventport.h"

#include "../controlmessage/controlmessage.pb.h"
#include "../controlmessage/translate.h"



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
        if(CloseLibrary_(lib_handle)){
            std::cout << "DLL Closed: " << it->first << std::endl;
        }else{
            std::cout << "DLL Error: "  << it->first << ": " << GetLibraryError() << std::endl;
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
    if(component && component->is_active()){
        return component->Passivate();
    }
    return false;
}

EventPort* NodeContainer::ConstructPeriodicEvent(Component* component, std::string port_name){
    return new PeriodicEventPort(component, port_name, component->GetCallback(port_name), 1000);
}

void NodeContainer::Configure(NodeManager::ControlMessage* message){
    auto n = message->mutable_node();

    for(auto c : n->components()){
        auto c_info = c.mutable_info();
        auto component = GetComponent(c_info->name());

        if(!component){
            //Construct Component
            component = ConstructComponent(c_info->type(), c_info->name());

            //Set features on component
            component->set_id(c_info->id());
            component->set_type(c_info->type());
        }

        if(component){
            for(auto a: c.attributes()){
                auto a_info = a.mutable_info();
                auto attribute = component->GetAttribute(a_info->name());
                if(attribute){
                    //std::cout << "Component: '" << a_info->name() << "' Setting Attribute: '" << a_info->name() << "'" <<  std::endl;
                    SetAttributeFromPb(&a, attribute);
                }
            }
            for(auto p : c.ports()){
                auto p_info = p.mutable_info();
                auto port = component->GetEventPort(p_info->name());

                //Get the middleware
                std::string middleware = NodeManager::EventPort_Middleware_Name(p.middleware());

                if(!port){
                    switch(p.kind()){
                        case NodeManager::EventPort::IN_PORT:{
                            port = ConstructRx(middleware, p_info->type(), component, p_info->name(), p.namespace_name());
                            break;
                        }
                        case NodeManager::EventPort::OUT_PORT:{
                            port = ConstructTx(middleware,p_info->type(), component, p_info->name(), p.namespace_name());
                            break;
                        }
                        case NodeManager::EventPort::PERIODIC_PORT:{
                            port = ConstructPeriodicEvent(component, p_info->name());
                            break;
                        }
                        default:
                            break;
                    }
                }

                //Configure the port
                if(port){
                    port->set_id(p_info->id());
                    port->set_type(p_info->type());

                    std::map<std::string, ::Attribute*> attributes_;

                    for(auto a: p.attributes()){
                        auto att = SetAttributeFromPb(&a);
                        if(att){
                            attributes_[att->get_name()] = att;
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
        c.second->Activate();
    }
    return true;
}

bool NodeContainer::PassivateAll(){
    //Passivate
    for(auto c : components_){
        c.second->Passivate();
    }
    //Clean up 
    for(auto c : components_){
        c.second->Teardown();
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
        //Set the library path
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

void* NodeContainer::LoadLibrary_(std::string library_path){
    //If we haven't seen the library_path before, try and load it.
    if(!loaded_libraries_.count(library_path)){
        auto start = std::chrono::steady_clock::now();

        void* lib_handle = 0;
        #ifdef _WIN32
            lib_handle = LoadLibrary(library_path.c_str());
        #else
            //Get a handle to the dynamically linked library
            lib_handle = dlopen(library_path.c_str(), RTLD_LAZY);
        #endif

        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "* Loaded DLL: '" << library_path <<  "' In: " << ms.count() << " us" << std::endl;

        //Check for errors
        std::string error = GetLibraryError();
        if(!error.empty()){
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

bool NodeContainer::CloseLibrary_(void* lib){
    //If we haven't seen the library_path before, try and load it.
    bool closed = false;
    if(lib){
        #ifdef _WIN32
            closed = FreeLibrary((HMODULE)lib);
        #else
            closed = dlclose(lib) == 0;
        #endif
    }
    return closed;
}
std::string NodeContainer::GetLibraryError(){
    std::string message;
    #ifdef _WIN32
        DWORD error_id = ::GetLastError();
        if(!error_id){
            return message;
        }
        LPSTR buffer = nullptr;
        //Fill the buffer and get the size.
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);
        //Copy into our return parameter
        message = std::string(buffer, size);

        //Free the buffer.
        LocalFree(buffer);
    #else
        char* error = dlerror();
        if(error){
            message = std::string(error);
        }
    #endif
    return message;
}

void* NodeContainer::GetLibraryFunction_(std::string library_path, std::string function_name){
    void* lib_handle = LoadLibrary_(library_path);
    return GetLibraryFunction_(lib_handle, function_name);
}

void* NodeContainer::GetLibraryFunction_(void* lib_handle, std::string function_name){
    if(lib_handle){
        //Clear the library error
        GetLibraryError();
        
        auto start = std::chrono::steady_clock::now();
        void* function = 0;
        #ifdef _WIN32
            function = (void*)GetProcAddress((HMODULE)lib_handle, function_name.c_str());
        #else
            function = dlsym(lib_handle, function_name.c_str());
        #endif
        
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "* Loaded DLL Function: '" << function_name <<  "' In: " << ms.count() << " us" << std::endl;
        
        auto error = GetLibraryError();
        if(function && error.empty()){
            return function;
        }else{
            std::cerr << "DLL Error Linking '" + function_name + "': " << error << std::endl;
        }
    }
    return 0;
}

std::string get_port_binary(std::string middleware, std::string namespace_name, std::string datatype){
    std::string p = middleware + "_";
    if(!namespace_name.empty()){
        p += namespace_name + "_";
    }
    p += datatype;
    
    return to_lower(p);
}

EventPort* NodeContainer::ConstructTx(std::string middleware, std::string datatype, Component* component, 
                                      std::string port_name, std::string namespace_name){

    auto port_lib = get_port_binary(middleware, namespace_name, datatype);

    if(!tx_constructors_.count(port_lib)){
        //auto lib_path = library_path_ + "/libports_" + to_lower(middleware) + GetLibraryExtension();
        auto lib_path = library_path_ + "/" + GetLibraryPrefix() + port_lib + GetLibrarySuffix();

        //Get the function
        void* function = GetLibraryFunction_(lib_path, "ConstructTx");
        if(function){
            //Cast as EventPort* ConstructRx(std::string, std::string, Component*)
            auto typed_function = (EventPort* (*) (std::string, Component*)) function;
            //Add to the lookup
            tx_constructors_[port_lib] = typed_function;
        }
    }

    if(tx_constructors_.count(port_lib)){
        return tx_constructors_[port_lib](port_name, component);
    }
    return 0;
}

EventPort* NodeContainer::ConstructRx(std::string middleware, std::string datatype, Component* component,
                                      std::string port_name, std::string namespace_name){
    
    auto port_lib = get_port_binary(middleware, namespace_name, datatype);
    if(!rx_constructors_.count(port_lib)){
        auto lib_path = library_path_ + "/" + GetLibraryPrefix() + port_lib + GetLibrarySuffix();

        //Get the function
        void* function = GetLibraryFunction_(lib_path, "ConstructRx");
        if(function){
            //Cast as EventPort* ConstructRx(std::string, std::string, Component*)
            auto typed_function = (EventPort* (*) (std::string, Component*)) function;
            //Add to the lookup
            rx_constructors_[port_lib] = typed_function;
        }
    }

    if(rx_constructors_.count(port_lib)){
        return rx_constructors_[port_lib](port_name, component);
    }
    return 0;
}

std::string NodeContainer::GetLibrarySuffix() const{
     #ifdef _WIN32
        return ".dll";
    #else
        return ".so";
    #endif
}

std::string NodeContainer::GetLibraryPrefix() const{
     #ifdef _WIN32
        return "";
    #else
        return "lib";
    #endif
}

Component* NodeContainer::ConstructComponent(std::string component_type, std::string component_name){
    Component* c = 0;
    if(!component_constructors_.count(component_type)){
        auto lib_path = library_path_ + "/" + GetLibraryPrefix() + "components_" + to_lower(component_type) + GetLibrarySuffix();

        //Get the function
        void* function = GetLibraryFunction_(lib_path, "ConstructComponent");
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


