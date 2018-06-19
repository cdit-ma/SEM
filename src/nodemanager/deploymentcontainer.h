#ifndef CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H
#define CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H

#include <unordered_map>
#include <functional>

#include <core/component.h>
#include <core/ports/libportexport.h>
#include <core/libcomponentexport.h>

#include <proto/controlmessage/controlmessage.pb.h>


#include "dllloader.h"

namespace NodeManager{
    class Component;
    class Port;
    class Worker;
    class Node;
    class Info;
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

        DllLoader dll_loader;
};
#endif //CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H