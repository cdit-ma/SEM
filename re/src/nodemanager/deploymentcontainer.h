#ifndef CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H
#define CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H

#include <unordered_map>
#include <functional>
#include <memory>

#include "component.h"
#include "ports/libportexport.h"
#include "libcomponentexport.h"


#include "loggers/logan_logger.h"
#include "loggers/print_logger.h"

#include "dllloader.h"
#include "loganclient.h"

namespace NodeManager{
    class Component;
    class Port;
    class Worker;
    class Container;
    class Info;
    class Logger;
};

// REVIEW (Mitch): use `using` for type alias
typedef std::function<PortCConstructor> PortConstructor;
typedef std::function<ComponentCConstructor> ComponentConstructor;

// REVIEW (Mitch): Add this to NodeManager or core namespace
// REVIEW (Mitch): Move this away from being an Activatable
class DeploymentContainer : public Activatable{
    public:
    // REVIEW (Mitch): document differences between two constructors. Implement deployment container
    //  config struct?
    DeploymentContainer(const std::string& experiment_name,
                        const std::string& host_name,
                        const std::string& library_path,
                        const NodeManager::Container& container);
    DeploymentContainer(const std::string& experiment_name, const std::string& host_name,  const std::string& library_path);
    // REVIEW (Mitch): Virtual destructors are many??
    ~DeploymentContainer();
    void Configure(const NodeManager::Container& container);

    // REVIEW (Mitch): weak_ptr and shared_ptr return types, why?
    std::weak_ptr<Component>
    AddComponent(std::unique_ptr<Component> component, const std::string& name);
    // REVIEW (Mitch): Indexing components by std::string seems like a bad idea. Move to using
    //  ComponentID type?
    //  Supply pretty print get_name(ComponentID) -> std::string for human readability.
    std::weak_ptr<Component> GetComponent(const std::string& name);
        std::shared_ptr<Component> RemoveComponent(const std::string& name);


        std::weak_ptr<LoganClient> AddLoganClient(std::unique_ptr<LoganClient> component, const std::string& id);
        std::weak_ptr<LoganClient> GetLoganClient(const std::string& id);
        std::shared_ptr<LoganClient> RemoveLoganClient(const std::string& id);

        void AddLoganLogger(std::unique_ptr<Logan::Logger> logan_logger);
    protected:
        void HandleActivate();
        void HandlePassivate();
        void HandleTerminate();
        void HandleConfigure();
    public:
        void SetLoggers(Activatable& entity);
    private:
        // REVIEW (Mitch): can be static member or free function. Name better than
        //  GetNamespaceString
        std::string GetNamespaceString(const NodeManager::Info& port);

        // REVIEW (Mitch): Get/Constructors???
        //  Elaboration, why do these functions configure, register and return a configured x?
        //  Upon review of call sites, these functions are regularly called without capturing a
        //  return value. Rework and annotate as [[nodiscard]] if non-void return type.
        //Get/Constructors
        // REVIEW (Mitch): are shared pointers needed here?
        std::shared_ptr<Worker> GetConfiguredWorker(std::shared_ptr<Component> component, const NodeManager::Worker& worker_pb);
        std::shared_ptr<Port> GetConfiguredPort(std::shared_ptr<Component> component, const NodeManager::Port& eventport_pb);
        
        std::shared_ptr<Component> GetConfiguredComponent(const NodeManager::Component& component_pb);
        std::shared_ptr<LoganClient> GetConfiguredLoganClient(const NodeManager::Logger& logger_pb);
        std::shared_ptr<LoganClient> ConstructLoganClient(const std::string& id);

        // REVIEW (Mitch): template parameterise these?
        // REVIEW (Mitch): Are shared pointers needed here?
        std::shared_ptr<Port> ConstructPeriodicPort(std::weak_ptr<Component> component, const std::string& port_name);
        std::shared_ptr<Port> ConstructPublisherPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_str);
        std::shared_ptr<Port> ConstructSubscriberPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_str);
        
        std::shared_ptr<Port> ConstructRequesterPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_str);
        std::shared_ptr<Port> ConstructReplierPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_str);

        std::shared_ptr<Component> ConstructComponent(const std::string& component_type, const std::string& component_name, const std::string& namespace_str, const std::string& component_id);

        // REVIEW (Mitch): These can be either static members or free functions.
        std::string get_port_library_name(const std::string& port_type, const std::string& middleware, const std::string& namespace_name, const std::string& datatype);
        std::string get_component_library_name(const std::string& component_type, const std::string& namespace_name);


        //Middleware -> construct functions
        // REVIEW (Mitch): what are these std::string keys?
        std::unordered_map<std::string, PortConstructor> publisher_port_constructors_;
        std::unordered_map<std::string, PortConstructor> subscriber_port_constructors_;
        std::unordered_map<std::string, PortConstructor> requester_port_constructors_;
        std::unordered_map<std::string, PortConstructor> replier_port_constructors_;

        std::unordered_map<std::string, ComponentConstructor> component_constructors_;

        // REVIEW (Mitch): This mutex is being used inconsistently.
        std::mutex component_mutex_;
        // REVIEW (Mitch): What are these std::string keys?
        // REVIEW (Mitch): Investigate if map key is ever invalid relative to class internal name
        std::unordered_map<std::string, std::shared_ptr<Component> > components_;
        std::unordered_map<std::string, std::shared_ptr<LoganClient> > logan_clients_;

        DllLoader dll_loader;

        std::unique_ptr<Logan::Logger> logan_logger_;

        // REVIEW (Mitch): std::filesystem path
        // REVIEW (Mitch): const data members are bad
        const std::string library_path_;
        const std::string experiment_name_;
        const std::string host_name_;
};
#endif //CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H