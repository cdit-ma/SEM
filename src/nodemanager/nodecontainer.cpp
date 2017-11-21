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

#include <core/worker.h>
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

std::shared_ptr<EventPort> NodeContainer::ConstructPeriodicEvent(std::weak_ptr<Component> weak_component, const std::string& port_name){
    auto component = weak_component.lock();
    if(component){
        auto u_ptr = std::unique_ptr<EventPort>(new PeriodicEventPort(weak_component, port_name, component->GetCallback(port_name), 1000));
        return component->AddEventPort(std::move(u_ptr)).lock();
    }
    return nullptr;
}

void NodeContainer::Configure(const NodeManager::ControlMessage& message){
    const auto& node = message.node();
    
    for(const auto& component_pb : node.components()){
        auto component = GetConfiguredComponent(component_pb);

        //Configure the Component
        if(!(component && component->Configure())){
            ModelLogger::get_model_logger()->LogFailedComponentConstruction(component_pb.info().type(), component_pb.info().name(), component_pb.info().id());
        }
    }
}

std::shared_ptr<Worker> NodeContainer::GetConfiguredWorker(std::shared_ptr<Component> component, const NodeManager::Worker& worker_pb){
    std::shared_ptr<Worker> worker;
    if(component){
        const auto& worker_info_pb = worker_pb.info();
        //Get the worker
        worker = component->GetWorker(worker_info_pb.name()).lock();

        if(worker){
            //Handle the attributes
            for(const auto& attr : worker_pb.attributes()){
                SetAttributeFromPb(worker, attr);
            }
        }
    }
    return worker;
}

std::shared_ptr<Component> NodeContainer::GetConfiguredComponent(const NodeManager::Component& component_pb){
    const auto& component_info_pb = component_pb.info();
     
    //Try and get the Component first
    auto component = GetSharedComponent(component_info_pb.id());
    if(!component){
        //Construct the Component
        component = ConstructComponent(component_info_pb.type(), component_info_pb.name(), component_info_pb.id());
    }

    if(component){
        //Handle the attributes
        for(const auto& attr : component_pb.attributes()){
            SetAttributeFromPb(component, attr);
        }

        //Handle the eventports
        for(const auto& port_pb : component_pb.ports()){
            auto event_port = GetConfiguredEventPort(component, port_pb);
        }

        //Handle the workers
        for(const auto& port_pb : component_pb.workers()){
            auto worker = GetConfiguredWorker(component, port_pb);
        }
    }
    return component;
}

std::shared_ptr<EventPort> NodeContainer::GetConfiguredEventPort(std::shared_ptr<Component> component, const NodeManager::EventPort& eventport_pb){
    std::shared_ptr<EventPort> eventport;

    if(component){
        const auto& eventport_info_pb = eventport_pb.info();
        const auto& middleware = NodeManager::EventPort_Middleware_Name(eventport_pb.middleware());
        
        //Try get the port
        eventport = component->GetEventPort(eventport_info_pb.name()).lock();
        

        if(!eventport){
            switch(eventport_pb.kind()){
                case NodeManager::EventPort::IN_PORT:{
                    eventport = ConstructRx(middleware, eventport_info_pb.type(), component, eventport_info_pb.name(), eventport_pb.namespace_name());
                    break;
                }
                case NodeManager::EventPort::OUT_PORT:{
                    eventport = ConstructTx(middleware, eventport_info_pb.type(), component, eventport_info_pb.name(), eventport_pb.namespace_name());
                    break;
                }
                case NodeManager::EventPort::PERIODIC_PORT:{
                    eventport = ConstructPeriodicEvent(component, eventport_info_pb.name());
                    break;
                }
                default:
                    break;
            }

            if(eventport){
                eventport->set_id(eventport_info_pb.id());
                eventport->set_type(eventport_info_pb.type());
            }
        }


        if(eventport){
            for(const auto& attr : eventport_pb.attributes()){
                SetAttributeFromPb(eventport, attr);
            }
        }else{
            ModelLogger::get_model_logger()->LogFailedPortConstruction(eventport_info_pb.type(), eventport_info_pb.name(), eventport_info_pb.id());
        }
    }

    return eventport;
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

std::string NodeContainer::get_port_library_name(const std::string& middleware, const std::string& namespace_name, const std::string& datatype){
    std::string p = middleware + "_";
    if(!namespace_name.empty()){
        p += namespace_name + "_";
    }
    p += datatype;
    
    return to_lower(p);
}
std::string NodeContainer::get_component_library_name(const std::string& component_type){
    std::string c = "components_" + component_type;
    return to_lower(c);
}

std::shared_ptr<EventPort> NodeContainer::ConstructTx(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string&  namespace_name){
    std::shared_ptr<EventPort> eventport;
    const auto& library_name = get_port_library_name(middleware, namespace_name, datatype);

    if(!tx_constructors_.count(library_name)){
        auto function = dll_loader.GetLibraryFunction<TxCConstructor>(library_path_, library_name, "ConstructTx");
        if(function){
            tx_constructors_[library_name] = function;
        }
    }

    if(tx_constructors_.count(library_name)){
        auto eventport_ptr = tx_constructors_[library_name](port_name, component);
        auto component_shared = component.lock();

        if(component_shared){
            eventport = component_shared->AddEventPort(std::unique_ptr<EventPort>(eventport_ptr)).lock();
        }else{
            eventport = std::shared_ptr<EventPort>(eventport_ptr);
        }
    }
    return eventport;
}

std::shared_ptr<EventPort> NodeContainer::ConstructRx(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string&  namespace_name){
    std::shared_ptr<EventPort> eventport;
    const auto& library_name = get_port_library_name(middleware, namespace_name, datatype);

    if(!rx_constructors_.count(library_name)){
        auto function = dll_loader.GetLibraryFunction<RxCConstructor>(library_path_, library_name, "ConstructRx");
        if(function){
            rx_constructors_[library_name] = function;
        }
    }

    if(rx_constructors_.count(library_name)){
        auto eventport_ptr = rx_constructors_[library_name](port_name, component);
        auto component_shared = component.lock();

        if(component_shared){
            eventport = component_shared->AddEventPort(std::unique_ptr<EventPort>(eventport_ptr)).lock();
        }else{
            eventport = std::shared_ptr<EventPort>(eventport_ptr);
        }
    }
    return eventport;
}

std::shared_ptr<Component> NodeContainer::ConstructComponent(const std::string& component_type, const std::string& component_name, const std::string& component_id){
    std::shared_ptr<Component> component;
    const auto& library_name = get_component_library_name(component_type);

    if(!component_constructors_.count(library_name)){
        auto function = dll_loader.GetLibraryFunction<ComponentCConstructor>(library_path_, library_name, "ConstructComponent");
        if(function){
            component_constructors_[library_name] = function;
        }
    }

    if(component_constructors_.count(library_name)){
        auto component_ptr = component_constructors_[library_name](component_name);
        if(component_ptr){
            component_ptr->set_id(component_id);
            component_ptr->set_type(component_type);
            //Add the Component to the NodeContainer
            component = AddComponent(std::unique_ptr<Component>(component_ptr), component_id).lock();
        }
    }
    return component;
}