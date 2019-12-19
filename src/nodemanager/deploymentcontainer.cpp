#include "deploymentcontainer.h"

#include <core/translate.h>
#include <core/worker.h>
#include <core/ports/periodicport.h>

#include <proto/controlmessage/controlmessage.pb.h>


#include <iostream>
#include <algorithm>
#include <string>
#include <list>
#include <thread>

#include <sstream>
#include <iterator>
#include <future>

//Converts std::string to lower
// REVIEW (Mitch): belongs in a util namespace/file
std::string to_lower(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// REVIEW (Mitch): replace params with config struct. Use std::filesystem path for library path
// REVIEW (Mitch): This constructor is not used, roll into other constructor
DeploymentContainer::DeploymentContainer(const std::string& experiment_name, const std::string& host_name,  const std::string& library_path):
    Activatable(Activatable::Class::DEPLOYMENT_CONTAINER),
    experiment_name_(experiment_name),
    library_path_(library_path),
    host_name_(host_name){

}

// REVIEW (Mitch): Document differences between constructors
//  Appears that this is just a convenience constructor that also calls configure.
DeploymentContainer::DeploymentContainer(const std::string& experiment_name, const std::string& host_name, const std::string& library_path, const NodeManager::Container& container):
    DeploymentContainer(experiment_name, host_name, library_path)
{
    Configure(container);
}

DeploymentContainer::~DeploymentContainer(){
    // REVIEW (Mitch): Specify that we're calling activatable's Terminate function.
    Terminate();
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

// REVIEW (Mitch): Silent failure if we already have one.
//  This is probably unused, hard to tell without checking codegen.
void DeploymentContainer::AddLoganLogger(std::unique_ptr<Logan::Logger> logan_logger){
    if(!logan_logger_){
        logan_logger_ = std::move(logan_logger);
    }
}

void DeploymentContainer::Configure(const NodeManager::Container& container){
    try{
        //Set once off information
        set_name(container.info().name());
        set_id(container.info().id());
        
        //Try and configure all components
        for(const auto& logger_pb : container.loggers()){
            switch(logger_pb.type()){
                case NodeManager::Logger::CLIENT:{
                    GetConfiguredLoganClient(logger_pb);
                    break;
                }
                case NodeManager::Logger::MODEL:{
                    //Setup logan logger
                    if(!logan_logger_){
                        try{
                            // REVIEW (Mitch): Improve this interface, remove enum casting. Remove
                            //  NotNeededException entirely.
                            logan_logger_ = std::unique_ptr<Logan::Logger>(new Logan::Logger(experiment_name_, host_name_, container.info().name(), container.info().id(), logger_pb.publisher_address(), logger_pb.publisher_port(), (Logger::Mode)logger_pb.mode()));
                        }catch(const Logan::Logger::NotNeededException&){
                            // REVIEW (Mitch): Perform this check before calling constructor.
                        }
                    }
                    break;
                }
                // REVIEW (Mitch): Use of default in switch case?
                default:
                    break;
            }
        }
        
        for(const auto& component : container.components()){
            // REVIEW (Mitch): Why does this return a weak_ptr if we're never capturing it?
            GetConfiguredComponent(component);
        }

        std::cout << "* Configured Slave as: " << get_name() << std::endl;
        Activatable::Configure();
    }catch(const std::runtime_error& e){
        //Teardown The Components we may have constructed
        for(const auto& component : container.components()){
            RemoveComponent(component.info().id());
        }
        throw e;
    }
}

void DeploymentContainer::SetLoggers(Activatable& entity){
    if(logan_logger_){
        entity.logger().AddLogger(*logan_logger_);
    }
    entity.logger().AddLogger(Print::Logger::get_logger());
}

// REVIEW (Mitch): This function should not be here. Components own workers, move functionality to
//  Component
std::shared_ptr<Worker> DeploymentContainer::GetConfiguredWorker(std::shared_ptr<Component> component, const NodeManager::Worker& worker_pb){
    if(component){
        const auto& worker_info_pb = worker_pb.info();
        const auto& worker_name = worker_info_pb.name();

        //Get the worker
        auto worker = component->GetWorker(worker_name).lock();

        if(worker){
            SetLoggers(*worker);

            //Attach the ID
            worker->set_id(worker_info_pb.id());

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

// REVIEW (Mitch): This should be split into Get and Configure Component. If, for whatever reason,
//  we want to keep GetConfiguredComponent it should be composed of Get and Configure Component
// REVIEW (Mitch): Construction is also handled here, third function probably needed. Helper
//  function more justified.
std::shared_ptr<Component> DeploymentContainer::GetConfiguredComponent(const NodeManager::Component& component_pb){
    const auto& component_info_pb = component_pb.info();
     
    //Try and get the Component first
    auto component = GetComponent(component_info_pb.id()).lock();
    if(!component){
        //Construct the Component
        component = ConstructComponent(component_info_pb.type(), component_info_pb.name(), GetNamespaceString(component_info_pb), component_info_pb.id());
        
        //Set the once off information
        if(component){
            SetLoggers(*component);
            const auto& location = component_pb.location();
            const auto& replicate_indices = component_pb.replicate_indices();
            component->SetLocation({location.begin(), location.end()});
            component->SetReplicationIndices({replicate_indices.begin(), replicate_indices.end()});
            component->SetExperimentName(experiment_name_);
        }
    }


    if(component){
        //Handle the attributes
        for(const auto& attr_pb : component_pb.attributes()){
            SetAttributeFromPb(*component, attr_pb.second);
        }

        //Handle the ports
        for(const auto& port_pb : component_pb.ports()){
            auto port = GetConfiguredPort(component, port_pb);
            try{
                if(port->get_state() != StateMachine::State::NOT_CONFIGURED){
                    // REVIEW (Mitch): UHHHHHHHH WHAT??
                    //  Why do we do a full tour of our state machine?
                    //Terminate and reactivate?
                    std::cerr << "* Reconfiguring Port: " << port->get_name() << std::endl;
                    port->Terminate();
                    port->Configure();
                    port->Activate();
                }
            }catch(const std::exception& ex){
                std::cerr << "* Reconfiguring Port Failed: " << port->get_name() << " " << ex.what() << std::endl;
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

    // REVIEW (Mitch): unreachable code
    return nullptr;
}

// REVIEW (Mitch): As with above, split into multiple functions; Get, Configure, Construct
//  Supply helper function to expose legacy behaviour
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

// REVIEW (Mitch): Break into Get, Configure, Construct
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
                // REVIEW (Mitch): Review use of default switch cases.
                default:
                    break;
            }

            if(port){
                SetLoggers(*port);

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
    // REVIEW (Mitch): Why do we take this lock here, but not in HandleActivate or HandlePassivate?
    std::lock_guard<std::mutex> component_lock(component_mutex_);

    // REVIEW (Mitch): Does this order matter? Probably
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
    // REVIEW (Mitch): Does this order matter? Probably
    components_.clear();
    logan_clients_.clear();

    if(logan_logger_){
        logan_logger_.reset();
    }
}

void DeploymentContainer::HandleConfigure(){
    // REVIEW (Mitch): Citation needed
    // REVIEW (Mitch): Justified by arbitrary 1 second sleeps in some middleware ports.
    //Using async allows concurrent configuration of components, which gives orders of magnitude improvements
    // REVIEW (Mitch): success is unused
    auto success = true;
    std::lock_guard<std::mutex> component_lock(component_mutex_);

    // REVIEW (Mitch): This becomes a list of std::future<void> upon Activatable interface change.
    //  Signal error by setting exception on future, catch on future.get() call.
    std::list<std::future<bool> > results;

    for(const auto& p : components_){
        auto& component = p.second;
        // REVIEW (Mitch): Why are we doing a pointer validity check here but not for
        //  logan_clients_?
        if(component){
            //Construct a thread to run the terminate function, which is blocking
            results.push_back(std::async(std::launch::async, &Activatable::Configure, component));
        }
    }

    for(const auto& c : logan_clients_){
        results.push_back(std::async(std::launch::async, &Activatable::Configure, c.second));
    }

    // REVIEW (Mitch): This loop needs to complete even if we get an exception. Un-joined std::async
    //  destructor calls std::terminate.
    // XXX: This is a hard std::terminate call if we ever throw while other std::threads are in a
    //  joinable state.
    for(auto& result : results){
        try{
            // REVIEW (Mitch): Use boolean short-circuiting here?
            if(result.valid()){
                if(!result.get()){
                    throw std::runtime_error("DeploymentContainer failed to Configure Component");
                }
            }
        }catch(const std::exception& ex){
            // REVIEW (Mitch): What happens to incomplete futures/threads if one throws?
            throw;
        }
    }
}

std::weak_ptr<Component> DeploymentContainer::AddComponent(std::unique_ptr<Component> component, const std::string& name){
    std::lock_guard<std::mutex> component_lock(component_mutex_);
    if(component && components_.count(name) == 0){
        // REVIEW (Mitch): Change this to using insert with structured binding capture.
        //  auto [ref, success] = components_.insert(name, component);
        //  also check success.
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

// REVIEW (Mitch): Replace check, lookup and nullptr return with components_.at()
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

// REVIEW (Mitch): These can be constexpr if they exist in a header
// REVIEW (Mitch): This is a helper function that should live somewhere else.
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

// REVIEW (Mitch): Investigate reduction of code repetition
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
        // REVIEW (Mitch): This can throw from two function calls deeper. We aren't catching here,
        //  where are we catching?
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
