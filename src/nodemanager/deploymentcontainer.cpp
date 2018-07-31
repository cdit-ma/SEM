#include "deploymentcontainer.h"

#include <core/translate.h>
#include <core/worker.h>
#include <core/modellogger.h>
#include <core/ports/periodicport.h>

#include <re_common/proto/controlmessage/controlmessage.pb.h>

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

std::shared_ptr<Port> DeploymentContainer::ConstructPeriodicPort(std::weak_ptr<Component> weak_component, const std::string& port_name){
    auto component = weak_component.lock();
    if(component){
        try{
            const auto& callback = component->GetCallback<void, BaseMessage>(port_name);
            auto u_ptr = std::unique_ptr<Port>(new PeriodicPort(weak_component, port_name, callback, 1000));
            return component->AddPort(std::move(u_ptr)).lock();
        }catch(const std::exception& ex){
            throw std::runtime_error("Component: '" + component->get_name() + "' Cannot construct Periodic Port '" + ex.what());
        }
    }
    return nullptr;
}

bool DeploymentContainer::Configure(const NodeManager::Node& node){
    try{
        set_name(node.info().name());
        set_id(node.info().id());
        
        //Try and configure all components
        for(const auto& component_pb : node.components()){
            GetConfiguredComponent(component_pb);
        }

        //Try and configure all components
        for(const auto& logger_pb : node.loggers()){
            switch(logger_pb.type()){
                case NodeManager::Logger::CLIENT:{
                    GetConfiguredLoganClient(logger_pb);
                    break;
                }
                case NodeManager::Logger::MODEL:{
                    if(!ModelLogger::is_logger_setup()){
                        ModelLogger::setup_model_logger(node.info().name(), logger_pb.publisher_address(), logger_pb.publisher_port(), (ModelLogger::Mode)logger_pb.mode());
                    }
                    break;
                }
                default:
                    break;
            }
        }

        std::cout << "* Configured Slave as: " << get_name() << std::endl;
        return Activatable::Configure();
    }catch(const std::runtime_error& e){
        //Teardown The Components we may have constructed
        for(const auto& component_pb : node.components()){
            RemoveComponent(component_pb.info().id());
        }
        throw e;
    }
    return false;
}

std::shared_ptr<Worker> DeploymentContainer::GetConfiguredWorker(std::shared_ptr<Component> component, const NodeManager::Worker& worker_pb){
    if(component){
        const auto& worker_info_pb = worker_pb.info();
        const auto& worker_name = worker_info_pb.name();

        //Get the worker
        auto worker = component->GetWorker(worker_name).lock();

        if(worker){
            //Handle the attributes
            for(const auto& attr : worker_pb.attributes()){
                SetAttributeFromPb(*worker, attr.second);
            }
        }else{
            throw std::runtime_error("Component: '" + component->get_name() + "' Doesn't have a Worker: '" + worker_name + "'");
        }
    }
    return nullptr;
}

std::shared_ptr<Component> DeploymentContainer::GetConfiguredComponent(const NodeManager::Component& component_pb){
    const auto& component_info_pb = component_pb.info();
     
    //Try and get the Component first
    auto component = GetComponent(component_info_pb.id()).lock();
    if(!component){
        //Construct the Component
        component = ConstructComponent(component_info_pb.type(), component_info_pb.name(), GetNamespaceString(component_info_pb), component_info_pb.id());
    }

    if(component){
        //Handle the attributes
        for(const auto& attr_pb : component_pb.attributes()){
            SetAttributeFromPb(*component, attr_pb.second);
        }

        //Handle the ports
        for(const auto& port_pb : component_pb.ports()){
            auto port = GetConfiguredPort(component, port_pb);
            if(port->get_state() != Activatable::State::NOT_CONFIGURED){
                //Terminate and reactivate?
                std::cerr << "* Reconfiguring Port: " << port->get_name() << std::endl;
                port->Terminate();
                port->Configure();
                port->Activate();
            }
        }

        //Handle the workers
        for(const auto& worker_pb : component_pb.workers()){
            GetConfiguredWorker(component, worker_pb);
        }

        return component;
    }else{
        throw std::runtime_error("Cannot Construct Component: " + component_info_pb.name());
    }
    return nullptr;
}

std::shared_ptr<LoganClient> DeploymentContainer::GetConfiguredLoganClient(const NodeManager::Logger& logger_pb){
    if(logger_pb.type() == NodeManager::Logger::CLIENT && logger_pb.mode() != NodeManager::Logger::OFF){
        //Try and get the Component first
        auto logan_container = GetLoganClient(logger_pb.id()).lock();
        if(!logan_container){
            //Construct the Component
            logan_container = ConstructLoganClient(logger_pb.id());
        }

        if(logan_container){
            logan_container->SetEndpoint(logger_pb.publisher_address(), logger_pb.publisher_port());
            logan_container->SetFrequency(logger_pb.frequency());

            for(const auto& process : logger_pb.processes()){
                logan_container->AddProcess(process);
            }

            logan_container->SetLiveMode(logger_pb.mode() == NodeManager::Logger::LIVE);
        }else{
            throw std::runtime_error("Cannot Construct Logan Client: " + logger_pb.id());
        }

        return logan_container;
    }
    return nullptr;

}

std::shared_ptr<LoganClient> DeploymentContainer::ConstructLoganClient(const std::string& id){
    auto logan_client = std::unique_ptr<LoganClient>(new LoganClient(id));
    if(logan_client){
        return AddLoganClient(std::move(logan_client), id).lock();
    }
    return nullptr;
}

std::weak_ptr<LoganClient> DeploymentContainer::GetLoganClient(const std::string& id){
    std::lock_guard<std::mutex> component_lock(component_mutex_);
    if(logan_clients_.count(id)){
        return logan_clients_[id];
    }else{
        return std::weak_ptr<LoganClient>();
    }
}

std::shared_ptr<LoganClient> DeploymentContainer::RemoveLoganClient(const std::string& id){
    std::lock_guard<std::mutex> component_lock(component_mutex_);
    if(logan_clients_.count(id)){
        auto logan_client = logan_clients_[id];
        logan_clients_.erase(id);
        return logan_client;
    }
    return nullptr;
}



std::string DeploymentContainer::GetNamespaceString(const NodeManager::Info& info){
    std::ostringstream concatenated_stream;
    
    const auto& namespaces = info.namespaces();
    for(int i = 0; i < namespaces.size(); i++){
        concatenated_stream << namespaces.Get(i);
        if(i + 1 < namespaces.size()){
            concatenated_stream << '_';
        }
    }
    

    return concatenated_stream.str();
}

std::shared_ptr<Port> DeploymentContainer::GetConfiguredPort(std::shared_ptr<Component> component, const NodeManager::Port& port_pb){
    if(component){
        
        const auto& port_info_pb = port_pb.info();
        const auto& middleware = NodeManager::Middleware_Name(port_pb.middleware());
        const auto namespace_str = GetNamespaceString(port_pb.info());
        
        //Try get the port
        auto port = component->GetPort(port_info_pb.name()).lock();

        if(!port){
            switch(port_pb.kind()){
                case NodeManager::Port::PERIODIC:{
                    port = ConstructPeriodicPort(component, port_info_pb.name());
                    break;
                }
                case NodeManager::Port::PUBLISHER:{
                    port = ConstructPublisherPort(middleware, port_info_pb.type(), component, port_info_pb.name(), namespace_str);
                    break;
                }
                case NodeManager::Port::SUBSCRIBER:{
                    port = ConstructSubscriberPort(middleware, port_info_pb.type(), component, port_info_pb.name(), namespace_str);
                    break;
                }
                case NodeManager::Port::REQUESTER:{
                    port = ConstructRequesterPort(middleware, port_info_pb.type(), component, port_info_pb.name(), namespace_str);
                    break;
                }
                case NodeManager::Port::REPLIER:{
                    port = ConstructReplierPort(middleware, port_info_pb.type(), component, port_info_pb.name(), namespace_str);
                    break;
                }
                default:
                    break;
            }

            if(port){
                //Set the ID/TYPE Once
                port->set_id(port_info_pb.id());
                port->set_type(port_info_pb.type());
            }
        }

        if(port){
            for(const auto& attr : port_pb.attributes()){
                SetAttributeFromPb(*port, attr.second);
            }
            return port;
        }else{
            throw std::runtime_error("Cannot Construct Port: " + port_info_pb.name() + " " + port_info_pb.type() + " " + port_info_pb.id());
        }
    }

    return nullptr;
}

void DeploymentContainer::HandleActivate(){
    for(const auto& c : components_){
        c.second->Activate();
    }

    for(const auto& c : logan_clients_){
        c.second->Activate();
    }
}

void DeploymentContainer::HandlePassivate(){
    for(const auto& c : components_){
        c.second->Passivate();
    }

    for(const auto& c : logan_clients_){
        c.second->Passivate();
    }
}

void DeploymentContainer::HandleTerminate(){
    HandlePassivate();
    std::lock_guard<std::mutex> component_lock(component_mutex_);
    
    for(const auto& p : components_){
        auto& component = p.second;
        if(component){
            component->Terminate();
        }
    }
    

    for(const auto& p : logan_clients_){
        auto& logan_client = p.second;
        if(logan_client){
            logan_client->Terminate();
        }
    }
    
    components_.clear();
    logan_clients_.clear();
}

void DeploymentContainer::HandleConfigure(){
    //Using async allows concurrent configuration of components, which gives orders of magnitude improvements
    auto success = true;
    std::lock_guard<std::mutex> component_lock(component_mutex_);

    std::list<std::future<bool> > results;

    for(const auto& p : components_){
        auto& component = p.second;
        if(component){
            //Construct a thread to run the terminate function, which is blocking
            results.push_back(std::async(std::launch::async, &Activatable::Configure, component));
        }
    }

    for(const auto& c : logan_clients_){
        results.push_back(std::async(std::launch::async, &Activatable::Configure, c.second));
    }
    
    for(auto& result : results){
        if(!result.get()){
            throw std::runtime_error("DeploymentContainer failed to Configure Component");
        }
    }
}

std::weak_ptr<Component> DeploymentContainer::AddComponent(std::unique_ptr<Component> component, const std::string& name){
    std::lock_guard<std::mutex> component_lock(component_mutex_);
    if(component && components_.count(name) == 0){
        components_[name] = std::move(component);
        return components_[name];
    }else if(component){
        throw std::runtime_error("DeploymentContainer already has a Component with name: " + name);
    }else{
        throw std::runtime_error("DeploymentContainer cannot add a Null Component with name: " + name);
    }
}

std::weak_ptr<LoganClient> DeploymentContainer::AddLoganClient(std::unique_ptr<LoganClient> logan_client, const std::string& id){
    std::lock_guard<std::mutex> component_lock(component_mutex_);
    if(logan_client && logan_clients_.count(id) == 0){
        logan_clients_[id] = std::move(logan_client);
        return logan_clients_[id];
    }else if(logan_client){
        throw std::runtime_error("DeploymentContainer already has a Logan Client with ID: " + id);
    }else{
        throw std::runtime_error("DeploymentContainer cannot add a Null Logan Client with ID: " + id);
    }
}

std::weak_ptr<Component> DeploymentContainer::GetComponent(const std::string& name){
    std::lock_guard<std::mutex> component_lock(component_mutex_);
    if(components_.count(name)){
        return components_[name];
    }else{
        return std::weak_ptr<Component>();
    }
}

std::shared_ptr<Component> DeploymentContainer::RemoveComponent(const std::string& name){
    std::lock_guard<std::mutex> component_lock(component_mutex_);
    if(components_.count(name)){
        auto component = components_[name];
        components_.erase(name);
        return component;
    }
    return nullptr;
}

std::string DeploymentContainer::get_port_library_name(const std::string& port_type, const std::string& middleware, const std::string& namespace_str, const std::string& datatype){
    std::string p = "port_";
    p += port_type + "_";
    p += middleware + "_";
    if(!namespace_str.empty()){
        p += namespace_str + "_";
    }
    p += datatype;
    
    return to_lower(p);
}
std::string DeploymentContainer::get_component_library_name(const std::string& component_type, const std::string& namespace_str){
    std::string c = "component_" ;
    if(!namespace_str.empty()){
        c += namespace_str + "_";
    }
    c += component_type;
    return to_lower(c);
}

std::shared_ptr<Port> DeploymentContainer::ConstructPublisherPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string&  namespace_name){
    std::shared_ptr<Port> port;
    const auto& library_name = get_port_library_name("pubsub", middleware, namespace_name, datatype);

    if(!publisher_port_constructors_.count(library_name)){
        auto function = dll_loader.GetLibraryFunction<PortCConstructor>(library_path_, library_name, "ConstructPublisherPort");
        if(function){
            publisher_port_constructors_[library_name] = function;
        }
    }

    if(publisher_port_constructors_.count(library_name)){
        auto port_ptr = publisher_port_constructors_[library_name](port_name, component);
        auto component_shared = component.lock();

        if(component_shared){
            port = component_shared->AddPort(std::unique_ptr<Port>(port_ptr)).lock();
        }else{
            port = std::shared_ptr<Port>(port_ptr);
        }
    }
    return port;
}

std::shared_ptr<Port> DeploymentContainer::ConstructRequesterPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string&  namespace_name){
    std::shared_ptr<Port> port;
    const auto& library_name = get_port_library_name("requestreply", middleware, namespace_name, datatype);

    if(!requester_port_constructors_.count(library_name)){
        auto function = dll_loader.GetLibraryFunction<PortCConstructor>(library_path_, library_name, "ConstructRequesterPort");
        if(function){
            requester_port_constructors_[library_name] = function;
        }
    }

    if(requester_port_constructors_.count(library_name)){
        auto port_ptr = requester_port_constructors_[library_name](port_name, component);
        auto component_shared = component.lock();

        if(component_shared){
            port = component_shared->AddPort(std::unique_ptr<Port>(port_ptr)).lock();
        }else{
            port = std::shared_ptr<Port>(port_ptr);
        }
    }
    return port;
}

std::shared_ptr<Port> DeploymentContainer::ConstructReplierPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string&  namespace_name){
    std::shared_ptr<Port> port;
    const auto& library_name = get_port_library_name("requestreply", middleware, namespace_name, datatype);

    if(!replier_port_constructors_.count(library_name)){
        auto function = dll_loader.GetLibraryFunction<PortCConstructor>(library_path_, library_name, "ConstructReplierPort");
        if(function){
            replier_port_constructors_[library_name] = function;
        }
    }

    if(replier_port_constructors_.count(library_name)){
        auto port_ptr = replier_port_constructors_[library_name](port_name, component);
        auto component_shared = component.lock();

        if(component_shared){
            port = component_shared->AddPort(std::unique_ptr<Port>(port_ptr)).lock();
        }else{
            port = std::shared_ptr<Port>(port_ptr);
        }
    }
    return port;
}

std::shared_ptr<Port> DeploymentContainer::ConstructSubscriberPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string&  namespace_name){
    std::shared_ptr<Port> port;
    const auto& library_name = get_port_library_name("pubsub", middleware, namespace_name, datatype);

    if(!subscriber_port_constructors_.count(library_name)){
        auto function = dll_loader.GetLibraryFunction<PortCConstructor>(library_path_, library_name, "ConstructSubscriberPort");
        if(function){
            subscriber_port_constructors_[library_name] = function;
        }
    }

    if(subscriber_port_constructors_.count(library_name)){
        auto port_ptr = subscriber_port_constructors_[library_name](port_name, component);
        auto component_shared = component.lock();

        if(component_shared){
            port = component_shared->AddPort(std::unique_ptr<Port>(port_ptr)).lock();
        }else{
            port = std::shared_ptr<Port>(port_ptr);
        }
    }
    return port;
}

std::shared_ptr<Component> DeploymentContainer::ConstructComponent(const std::string& component_type, const std::string& component_name, const std::string& namespace_str, const std::string& component_id){
    std::shared_ptr<Component> component;
    const auto& library_name = get_component_library_name(component_type, namespace_str);

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
