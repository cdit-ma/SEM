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

#include <core/modellogger.h>
#include <core/eventports/periodiceventport.h>

#include "controlmessage/controlmessage.pb.h"
#include "controlmessage/translate.h"



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
    std::cout << "* Closing " << loaded_libraries_.size() << " DLLs" << std::endl;
    for(auto it=loaded_libraries_.begin(); it!=loaded_libraries_.end();){
        auto lib_handle = it->second;
        if(CloseLibrary_(lib_handle)){
            //std::cout << "* Closed DLL: '"  << it->first << std::endl;
        }else{
            std::cout << "* Closed DLL Error: '"  << it->first << ": " << GetLibraryError() << std::endl;
        }
        it = loaded_libraries_.erase(it);
    }
}

bool NodeContainer::Activate(std::string component_id){
    auto component = GetSharedComponent(component_id);
    if(component){
        return component->Activate();
    }
    return false;
}

bool NodeContainer::Passivate(std::string component_id){
    auto component = GetSharedComponent(component_id);
    if(component){
        return component->Passivate();
    }
    return false;
}

std::shared_ptr<Component> NodeContainer::GetSharedComponent(const std::string& name){
    return GetComponent(name).lock();
}

std::shared_ptr<EventPort> NodeContainer::ConstructPeriodicEvent(std::weak_ptr<Component> weak_component, std::string port_name){
    auto component = weak_component.lock();
    if(component){
        auto u_ptr = std::unique_ptr<EventPort>(new PeriodicEventPort(weak_component, port_name, component->GetCallback(port_name), 1000));
        return component->AddEventPort(std::move(u_ptr)).lock();
    }
    return nullptr;
}

void NodeContainer::Configure(NodeManager::ControlMessage* message){
    auto n = message->mutable_node();

    for(const auto& c : n->components()){
        const auto& c_info = c.info();
        auto component = GetSharedComponent(c_info.id());

        if(!component){
            component = ConstructComponent(c_info.type(), c_info.name(), c_info.id());
        }

        if(component){
            //Set features on component
            component->set_id(c_info.id());
            component->set_type(c_info.type());

            //Set the Attributes
            for(const auto& a: c.attributes()){
                SetAttributeFromPb(component, a);
            }

            for(const auto& p : c.ports()){
                const auto& p_info = p.info();
                const auto& middleware = NodeManager::EventPort_Middleware_Name(p.middleware());

                auto port = component->GetEventPort(p_info.id()).lock();

                if(!port){
                    switch(p.kind()){
                        case NodeManager::EventPort::IN_PORT:{
                            port = ConstructRx(middleware, p_info.type(), component, p_info.name(), p.namespace_name());
                            break;
                        }
                        case NodeManager::EventPort::OUT_PORT:{
                            port = ConstructTx(middleware, p_info.type(), component, p_info.name(), p.namespace_name());
                            break;
                        }
                        case NodeManager::EventPort::PERIODIC_PORT:{
                            port = ConstructPeriodicEvent(component, p_info.name());
                            break;
                        }
                        default:
                            break;
                    }

                    //Only setup once
                    if(port){
                        port->set_id(p_info.id());
                        port->set_type(p_info.type());
                    }
                }
                
                //Configure the port
                if(port){
                    for(const auto& a : p.attributes()){
                        SetAttributeFromPb(port, a);
                    }
                }else{
                    ModelLogger::get_model_logger()->LogFailedPortConstruction(p_info.type(), p_info.name(), p_info.id());
                }
            }
        }

        if(!(component && component->Configure())){
            ModelLogger::get_model_logger()->LogFailedComponentConstruction(c_info.type(), c_info.name(), c_info.id());
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
        auto component = c.second;
        component->Passivate();
    }
    return true;
}
void NodeContainer::Teardown(){
    for(auto it=components_.begin(); it!=components_.end();){
        auto c = it->second;
        if(c && c->Terminate()){
            it = components_.erase(it);
        }
    }
}


std::weak_ptr<Component> NodeContainer::AddComponent(std::unique_ptr<Component> component, const std::string& name){
    if(components_.count(name) == 0){
        components_[name] = std::move(component);
        return components_[name];
    }else{
        std::cerr << "NodeContainer already has a Component with name '" << name << "'" << std::endl;
        return std::weak_ptr<Component>();
    }
}

std::weak_ptr<Component> NodeContainer::GetComponent(const std::string& name){
    if(components_.count(name)){
        return components_[name];
    }else{
        return std::weak_ptr<Component>();
    }
}

std::shared_ptr<Component> NodeContainer::RemoveComponent(const std::string& name){
    if(components_.count(name)){
        auto component = components_[name];
        components_.erase(name);
        return component;
    }else{ 
        std::cerr << "NodeContainer doesn't have a Component with name '" << name << "'" << std::endl;
        return std::shared_ptr<Component>();
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

        //Check for errors
        std::string error = GetLibraryError();
        if(!error.empty()){
            std::cerr << "DLL Error Loading: " << library_path << " : "<< error << std::endl;
        }else{
            //Add it to the map of loaded libraries
            std::cout << "* Loaded DLL: '" << library_path <<  "' In: " << ms.count() << " us" << std::endl;
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
        //std::cout << "* Loaded DLL Function: '" << function_name <<  "' In: " << ms.count() << " us" << std::endl;
        
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

std::shared_ptr<EventPort> NodeContainer::ConstructTx(std::string middleware, std::string datatype, std::weak_ptr<Component> component, 
                                      std::string port_name, std::string namespace_name){

    auto port_lib = get_port_binary(middleware, namespace_name, datatype);

    if(!tx_constructors_.count(port_lib)){
        //auto lib_path = library_path_ + "/libports_" + to_lower(middleware) + GetLibraryExtension();
        auto lib_path = library_path_ + "/" + GetLibraryPrefix() + port_lib + GetLibrarySuffix();

        //Get the function
        auto function = (TxCConstructor*) (GetLibraryFunction_(lib_path, "ConstructTx"));
        if(function){
            //Add to the lookup
            tx_constructors_[port_lib] = function;
        }
    }

    if(tx_constructors_.count(port_lib)){
        auto ep = tx_constructors_[port_lib](port_name, component);
        auto component_shared = component.lock();

        if(component_shared){
            return component_shared->AddEventPort(std::unique_ptr<EventPort>(ep)).lock();
        }
        return std::shared_ptr<EventPort>(ep);
    }
    return nullptr;
}

std::shared_ptr<EventPort> NodeContainer::ConstructRx(std::string middleware, std::string datatype, std::weak_ptr<Component> component,
                                      std::string port_name, std::string namespace_name){
    
    auto port_lib = get_port_binary(middleware, namespace_name, datatype);
    if(!rx_constructors_.count(port_lib)){
        auto lib_path = library_path_ + "/" + GetLibraryPrefix() + port_lib + GetLibrarySuffix();

        //Get the function
        auto function = (RxCConstructor*) GetLibraryFunction_(lib_path, "ConstructRx");
        if(function){
            //Add to the lookup
            rx_constructors_[port_lib] = function;
        }
    }

    if(rx_constructors_.count(port_lib)){
        auto ep = rx_constructors_[port_lib](port_name, component);
        
        auto component_shared = component.lock();

        if(component_shared){
            return component_shared->AddEventPort(std::unique_ptr<EventPort>(ep)).lock();
        }
        return std::shared_ptr<EventPort>(ep);
    }
    return nullptr;
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

std::shared_ptr<Component> NodeContainer::ConstructComponent(std::string component_type, std::string component_name, std::string component_id){
    if(!component_constructors_.count(component_type)){
        auto lib_path = library_path_ + "/" + GetLibraryPrefix() + "components_" + to_lower(component_type) + GetLibrarySuffix();

        //Get the function
        auto function = (ComponentCConstructor*) GetLibraryFunction_(lib_path, "ConstructComponent");
        if(function){
            //Add to the lookup
            component_constructors_[component_type] = function;
        }
    }

    if(component_constructors_.count(component_type)){
        auto component = component_constructors_[component_type](component_name);
        if(component){

            //Add the Component to the NodeContainer
            return AddComponent(std::unique_ptr<Component>(component), component_id).lock();
        }
    }
    return nullptr;
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

void NodeContainer::AddComponentConstructor(std::string component_type, ComponentConstructor constructor){
    if(!component_constructors_.count(component_type)){
        component_constructors_[component_type] = constructor;
    }
}
