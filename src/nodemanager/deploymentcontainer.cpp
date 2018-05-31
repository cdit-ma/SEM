#include "deploymentcontainer.h"

#include <core/worker.h>
#include <core/modellogger.h>
#include <core/eventports/periodiceventport.h>

#include "../core/translate.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <list>
#include <thread>

#include <sstream>
#include <iterator>
#include <future>

//Converts std::string to lower
std::string to_lower(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

DeploymentContainer::DeploymentContainer(){
    set_name("Deployment Container");
}

DeploymentContainer::~DeploymentContainer(){
    Terminate();
}

void DeploymentContainer::SetLibraryPath(const std::string library_path){
    library_path_ = library_path;
}

std::shared_ptr<EventPort> DeploymentContainer::ConstructPeriodicEvent(std::weak_ptr<Component> weak_component, const std::string& port_name){
    auto component = weak_component.lock();
    if(component){
        auto u_ptr = std::unique_ptr<EventPort>(new PeriodicEventPort(weak_component, port_name, component->GetCallback(port_name), 1000));
        return component->AddEventPort(std::move(u_ptr)).lock();
    }
    return nullptr;
}

bool DeploymentContainer::Configure(const NodeManager::Node& node){
    bool success = true;
    
    set_name(node.info().name());
    set_id(node.info().id());

    for(const auto& component_pb : node.components()){
        auto component = GetConfiguredComponent(component_pb);
        success = component ? success : false;
    }
    if(success){
        std::cout << "* Configured DeploymentContainer: " << get_name() << std::endl;
    }
    return success && Activatable::Configure();
}

std::shared_ptr<Worker> DeploymentContainer::GetConfiguredWorker(std::shared_ptr<Component> component, const NodeManager::Worker& worker_pb){
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

std::shared_ptr<Component> DeploymentContainer::GetConfiguredComponent(const NodeManager::Component& component_pb){
    const auto& component_info_pb = component_pb.info();
     
    //Try and get the Component first
    auto component = GetComponent(component_info_pb.id()).lock();
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

        //Handle the external classes

    }
    return component;
}

std::string DeploymentContainer::GetNamespaceString(const NodeManager::EventPort& port){
    std::ostringstream concatenated_stream;
    
    const auto& namespaces = port.namespaces();
    for(int i = 0; i < namespaces.size(); i++){
        concatenated_stream << namespaces.Get(i);
        if(i + 1 < namespaces.size()){
            concatenated_stream << '_';
        }
    }
    

    return concatenated_stream.str();
}

std::shared_ptr<EventPort> DeploymentContainer::GetConfiguredEventPort(std::shared_ptr<Component> component, const NodeManager::EventPort& eventport_pb){
    std::shared_ptr<EventPort> eventport;

    if(component){
        const auto& eventport_info_pb = eventport_pb.info();
        const auto& middleware = NodeManager::EventPort_Middleware_Name(eventport_pb.middleware());
        
        //Try get the port
        eventport = component->GetEventPort(eventport_info_pb.name()).lock();
        
        
        const auto namespace_str = GetNamespaceString(eventport_pb);

        if(!eventport){
            switch(eventport_pb.kind()){
                case NodeManager::EventPort::IN_PORT:{
                    eventport = ConstructInEventPort(middleware, eventport_info_pb.type(), component, eventport_info_pb.name(), namespace_str);
                    break;
                }
                case NodeManager::EventPort::OUT_PORT:{
                    eventport = ConstructOutEventPort(middleware, eventport_info_pb.type(), component, eventport_info_pb.name(), namespace_str);
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

bool DeploymentContainer::HandleActivate(){
    auto success = true;
    for(const auto& c : components_){
        success = c.second->Activate() ? success : false;
    }
    return true;
}

bool DeploymentContainer::HandlePassivate(){
    auto success = true;

    for(const auto& c : components_){
        success = c.second->Passivate() ? success : false;
    }

    return success;
}

bool DeploymentContainer::HandleTerminate(){
    //Using async allows concurrent termination of components, which gives orders of magnitude improvements
    auto success = true;
    std::list<std::future<bool> > results;

    for(const auto& c : components_){
        results.push_back(std::async(std::launch::async, &Activatable::Terminate, c.second));
    }
    components_.clear();

    for(auto& result : results){
        success &= result.get();
    }
    return success;
}

bool DeploymentContainer::HandleConfigure(){
    //Using async allows concurrent configuration of components, which gives orders of magnitude improvements
    auto success = true;
    std::list<std::future<bool> > results;

    for(const auto& c : components_){
        results.push_back(std::async(std::launch::async, &Activatable::Configure, c.second));
    }
    for(auto& result : results){
        success &= result.get();
    }
    return success;
}

std::weak_ptr<Component> DeploymentContainer::AddComponent(std::unique_ptr<Component> component, const std::string& name){
    if(components_.count(name) == 0){
        components_[name] = std::move(component);
        return components_[name];
    }else{
        std::cerr << "DeploymentContainer already has a Component with name '" << name << "'" << std::endl;
        return std::weak_ptr<Component>();
    }
}

std::weak_ptr<Component> DeploymentContainer::GetComponent(const std::string& name){
    if(components_.count(name)){
        return components_[name];
    }else{
        return std::weak_ptr<Component>();
    }
}

std::shared_ptr<Component> DeploymentContainer::RemoveComponent(const std::string& name){
    if(components_.count(name)){
        auto component = components_[name];
        components_.erase(name);
        return component;
    }else{ 
        std::cerr << "DeploymentContainer doesn't have a Component with name '" << name << "'" << std::endl;
        return std::shared_ptr<Component>();
    }
}

std::string DeploymentContainer::get_port_library_name(const std::string& middleware, const std::string& namespace_name, const std::string& datatype){
    std::string p = middleware + "_";
    if(!namespace_name.empty()){
        p += namespace_name + "_";
    }
    p += datatype;
    
    return to_lower(p);
}
std::string DeploymentContainer::get_component_library_name(const std::string& component_type){
    std::string c = "component_" + component_type;
    return to_lower(c);
}

std::shared_ptr<EventPort> DeploymentContainer::ConstructOutEventPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string&  namespace_name){
    std::shared_ptr<EventPort> eventport;
    const auto& library_name = get_port_library_name(middleware, namespace_name, datatype);

    if(!out_eventport_constructors_.count(library_name)){
        auto function = dll_loader.GetLibraryFunction<EventPortCConstructor>(library_path_, library_name, "ConstructOutEventPort");
        if(function){
            out_eventport_constructors_[library_name] = function;
        }
    }

    if(out_eventport_constructors_.count(library_name)){
        auto eventport_ptr = out_eventport_constructors_[library_name](port_name, component);
        auto component_shared = component.lock();

        if(component_shared){
            eventport = component_shared->AddEventPort(std::unique_ptr<EventPort>(eventport_ptr)).lock();
        }else{
            eventport = std::shared_ptr<EventPort>(eventport_ptr);
        }
    }
    return eventport;
}

std::shared_ptr<EventPort> DeploymentContainer::ConstructInEventPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string&  namespace_name){
    std::shared_ptr<EventPort> eventport;
    const auto& library_name = get_port_library_name(middleware, namespace_name, datatype);

    if(!in_eventport_constructors_.count(library_name)){
        auto function = dll_loader.GetLibraryFunction<EventPortCConstructor>(library_path_, library_name, "ConstructInEventPort");
        if(function){
            in_eventport_constructors_[library_name] = function;
        }
    }

    if(in_eventport_constructors_.count(library_name)){
        auto eventport_ptr = in_eventport_constructors_[library_name](port_name, component);
        auto component_shared = component.lock();

        if(component_shared){
            eventport = component_shared->AddEventPort(std::unique_ptr<EventPort>(eventport_ptr)).lock();
        }else{
            eventport = std::shared_ptr<EventPort>(eventport_ptr);
        }
    }
    return eventport;
}

std::shared_ptr<Component> DeploymentContainer::ConstructComponent(const std::string& component_type, const std::string& component_name, const std::string& component_id){
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
            //Add the Component to the DeploymentContainer
            component = AddComponent(std::unique_ptr<Component>(component_ptr), component_id).lock();
        }
    }
    return component;
}
