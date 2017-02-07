#ifndef CORE_NODEMANAGER_NODECONTAINER_H
#define CORE_NODEMANAGER_NODECONTAINER_H

#include <unordered_map>
#include <functional>

#include "../component.h"

namespace NodeManager{
    class ControlMessage;
};

typedef std::function<EventPort* (std::string, Component*)> TxConstructor;
typedef std::function<EventPort* (std::string, Component*)> RxConstructor;
typedef std::function<Component* (std::string)> ComponentConstructor;

class NodeContainer{ 
    public:
        NodeContainer(std::string library_path);
        ~NodeContainer();
        
        bool Activate(std::string component_name);
        bool Passivate(std::string component_name);

        bool ActivateAll();
        bool PassivateAll();

        void Configure(NodeManager::ControlMessage* message);
        
        void Teardown();

        bool AddComponent(Component* component);
        Component* GetComponent(std::string component_name);

    private:
        //DLL functions
        void* LoadLibrary(std::string dll_path);
        void* GetLibraryFunction(std::string dll_path, std::string function_name);
        void* GetLibraryFunction(void* lib_handle, std::string function_name);

        void AddTxConstructor(std::string middleware, TxConstructor constructor);
        void AddRxConstructor(std::string middleware, TxConstructor constructor);
        void AddComponentConstructor(std::string component_type, ComponentConstructor constructor);
    
    public:
        //Constructor functions
        EventPort* ConstructPeriodicEvent(Component* component, std::string port_name);
        EventPort* ConstructTx(std::string middleware, std::string datatype, Component* component, std::string port_name);
        EventPort* ConstructRx(std::string middleware, std::string datatype, Component* component, std::string port_name);

        Component* ConstructComponent(std::string component_type, std::string component_name);

     private:
        std::string library_path_;

        //Library Path -> void *
        std::unordered_map<std::string, void *> loaded_libraries_;

        //Middleware -> construct functions
        std::unordered_map<std::string, TxConstructor> tx_constructors_;
        std::unordered_map<std::string, RxConstructor> rx_constructors_;

        //Component Type -> construct functions
        std::unordered_map<std::string, ComponentConstructor> component_constructors_;
        
        //Map for quick insertion and stuffs.
        std::map<std::string, Component*> components_;
};
      

#endif //CORE_NODEMANAGER_NODECONTAINER_H