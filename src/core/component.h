#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <memory>
#include <iostream>

#include "ports/port.h"
class Worker;

#include "behaviourcontainer.h"
#include "basemessage.h"

class Component : public BehaviourContainer{
    public:
        Component(const std::string& component_name = "");
        virtual ~Component();

        //Port
        std::weak_ptr<Port> AddPort(std::unique_ptr<Port> port);
        std::weak_ptr<Port> GetPort(const std::string& port_name);
        template<class T>
        std::shared_ptr<T> GetTypedPort(const std::string& port_name);
        std::shared_ptr<Port> RemovePort(const std::string& port_name);
        
        template<class T>
        bool AddCallback(const std::string& port_name, std::function<void (T&)> function);
        
        bool AddPeriodicCallback(const std::string& port_name, std::function<void()> function);

        std::function<void (::BaseMessage&)> GetCallback(const std::string& port_name);
        bool RemoveCallback(const std::string& port_name);
    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();
    private:
        bool AddCallback_(const std::string& port_name, std::function<void (::BaseMessage&)> function);
        std::mutex state_mutex_;
        std::mutex port_mutex_;

        std::unordered_map<std::string, std::shared_ptr<Port> > ports_;
        std::unordered_map<std::string, std::function<void (::BaseMessage&)> > callback_functions_;
};

template<class T>
std::shared_ptr<T> Component::GetTypedPort(const std::string& port_name){
    static_assert(std::is_base_of<Port, T>::value, "T must inherit from Port");
    auto p = GetPort(port_name).lock();
    return std::dynamic_pointer_cast<T>(p);
};

template<class T>
bool Component::AddCallback(const std::string& port_name, std::function<void (T&)> function){
    static_assert(std::is_base_of<::BaseMessage, T>::value, "T must inherit from ::BaseMessage");

    return AddCallback_(port_name, [this, function] (::BaseMessage& message){
            function((T&) message);
        });
};



#endif //COMPONENT_H