#ifndef CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H
#define CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H

#include <unordered_map>
#include <functional>
#include <memory>

#include <core/component.h>
#include <core/ports/libportexport.h>
#include <core/libcomponentexport.h>




#include "dllloader.h"
#include "loganclient.h"

namespace NodeManager{
    class Component;
    class Port;
    class Worker;
    class Node;
    class Info;
    class Logger;
};

typedef std::function<PortCConstructor> PortConstructor;
typedef std::function<ComponentCConstructor> ComponentConstructor;

class DeploymentContainer : public Activatable{
    public:
        DeploymentContainer();
        ~DeploymentContainer();
        bool Configure(const NodeManager::Node& message);
        std::weak_ptr<Component> AddComponent(std::unique_ptr<Component> component, const std::string& name);
        std::weak_ptr<Component> GetComponent(const std::string& name);
        std::shared_ptr<Component> RemoveComponent(const std::string& name);


        std::weak_ptr<LoganClient> AddLoganClient(std::unique_ptr<LoganClient> component, const std::string& id);
        std::weak_ptr<LoganClient> GetLoganClient(const std::string& id);
        std::shared_ptr<LoganClient> RemoveLoganClient(const std::string& id);

        void SetLibraryPath(const std::string library_path);
    protected:
        bool HandleActivate();
        bool HandlePassivate();
        bool HandleTerminate();
        bool HandleConfigure();
    private:
        std::string GetNamespaceString(const NodeManager::Info& port);
        //Get/Constructors
        std::shared_ptr<Worker> GetConfiguredWorker(std::shared_ptr<Component> component, const NodeManager::Worker& worker_pb);
        std::shared_ptr<Port> GetConfiguredPort(std::shared_ptr<Component> component, const NodeManager::Port& eventport_pb);
        
        std::shared_ptr<Component> GetConfiguredComponent(const NodeManager::Component& component_pb);
        

        
        std::shared_ptr<LoganClient> GetConfiguredLoganClient(const NodeManager::Logger& logger_pb);
        std::shared_ptr<LoganClient> ConstructLoganClient(const std::string& id);
        

        
        
        
        //Constructor functions
        std::shared_ptr<Port> ConstructPeriodicPort(std::weak_ptr<Component> component, const std::string& port_name);
        
        std::shared_ptr<Port> ConstructPublisherPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_str);
        std::shared_ptr<Port> ConstructSubscriberPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_str);
        
        std::shared_ptr<Port> ConstructRequesterPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_str);
        std::shared_ptr<Port> ConstructReplierPort(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_str);

        std::shared_ptr<Component> ConstructComponent(const std::string& component_type, const std::string& component_name, const std::string& namespace_str, const std::string& component_id);

        std::string get_port_library_name(const std::string& port_type, const std::string& middleware, const std::string& namespace_name, const std::string& datatype);
        std::string get_component_library_name(const std::string& component_type, const std::string& namespace_name);

        std::string library_path_;

        //Middleware -> construct functions
        std::unordered_map<std::string, PortConstructor> publisher_port_constructors_;
        std::unordered_map<std::string, PortConstructor> subscriber_port_constructors_;
        std::unordered_map<std::string, PortConstructor> requester_port_constructors_;
        std::unordered_map<std::string, PortConstructor> replier_port_constructors_;

        std::unordered_map<std::string, ComponentConstructor> component_constructors_;

        std::mutex component_mutex_;
        std::unordered_map<std::string, std::shared_ptr<Component> > components_;
        std::unordered_map<std::string, std::shared_ptr<LoganClient> > logan_clients_;

        DllLoader dll_loader;
};
#endif //CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H