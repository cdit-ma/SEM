#ifndef CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H
#define CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H

#include <map>
#include <functional>

#include <core/component.h>
#include <core/libportexports.h>
#include <core/libcomponentexports.h>

#include "dllloader.h"

namespace NodeManager{
    class Component;
    class EventPort;
    class Worker;
    class Node;
};

typedef std::function<TxCConstructor> TxConstructor;
typedef std::function<RxCConstructor> RxConstructor;
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
        //Get/Constructors
        std::shared_ptr<Worker> GetConfiguredWorker(std::shared_ptr<Component> component, const NodeManager::Worker& worker_pb);
        std::shared_ptr<Component> GetConfiguredComponent(const NodeManager::Component& component_pb);
        std::shared_ptr<EventPort> GetConfiguredEventPort(std::shared_ptr<Component> component, const NodeManager::EventPort& eventport_pb);
        
        //Constructor functions
        std::shared_ptr<EventPort> ConstructPeriodicEvent(std::weak_ptr<Component> component, const std::string& port_name);
        std::shared_ptr<EventPort> ConstructTx(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_name);
        std::shared_ptr<EventPort> ConstructRx(const std::string& middleware, const std::string& datatype, std::weak_ptr<Component> component, const std::string& port_name, const std::string& namespace_name);
        std::shared_ptr<Component> ConstructComponent(const std::string& component_type, const std::string& component_name, const std::string& component_id);
        
        std::string get_port_library_name(const std::string& middleware, const std::string& namespace_name, const std::string& datatype);
        std::string get_component_library_name(const std::string& component_type);

        std::string library_path_;

        //Middleware -> construct functions
        std::map<std::string, TxConstructor> tx_constructors_;
        std::map<std::string, RxConstructor> rx_constructors_;
        std::map<std::string, ComponentConstructor> component_constructors_;
        
        //Map for quick insertion and stuffs.
        std::map<std::string, std::shared_ptr<Component> > components_;
        DllLoader dll_loader;
};
#endif //CORE_NODEMANAGER_DEPLOYMENTCONTAINER_H