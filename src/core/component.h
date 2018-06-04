#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <memory>
#include "eventports/eventport.h"
#include <iostream>

#include "behaviourcontainer.h"
#include "basemessage.h"

class Worker;
class EventPort;

class Component : public BehaviourContainer{
    public:
        Component(const std::string& component_name = "");
        virtual ~Component();

        //EventPort
        std::weak_ptr<EventPort> AddEventPort(std::unique_ptr<EventPort> event_port);
        std::weak_ptr<EventPort> GetEventPort(const std::string& event_port_name);
        template<class T>
        std::shared_ptr<T> GetTypedEventPort(const std::string& event_port_name);
        std::shared_ptr<EventPort> RemoveEventPort(const std::string& event_port_name);
        
        template<class T>
        bool AddCallback(const std::string& event_port_name, std::function<void (T&)> function);
        
        bool AddPeriodicCallback(const std::string& event_port_name, std::function<void()> function);

        std::function<void (::BaseMessage&)> GetCallback(const std::string& event_port_name);
        bool RemoveCallback(const std::string& event_port_name);
    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();
    private:
        bool AddCallback_(const std::string& event_port_name, std::function<void (::BaseMessage&)> function);
        std::mutex state_mutex_;
        std::mutex port_mutex_;

        std::unordered_map<std::string, std::shared_ptr<EventPort> > eventports_;
        std::unordered_map<std::string, std::function<void (::BaseMessage&)> > callback_functions_;
};

template<class T>
std::shared_ptr<T> Component::GetTypedEventPort(const std::string& event_port_name){
    static_assert(std::is_base_of<EventPort, T>::value, "T must inherit from EventPort");
    auto p = GetEventPort(event_port_name).lock();
    return std::dynamic_pointer_cast<T>(p);
};

template<class T>
bool Component::AddCallback(const std::string& event_port_name, std::function<void (T&)> function){
    static_assert(std::is_base_of<::BaseMessage, T>::value, "T must inherit from ::BaseMessage");

    return AddCallback_(event_port_name, [this, function] (::BaseMessage& message){
            function((T&) message);
        });
};



#endif //COMPONENT_H