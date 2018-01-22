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

#include "activatable.h"
#include "basemessage.h"

class Worker;
class EventPort;

class Component : public Activatable{
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
        std::shared_ptr<T> AddTypedWorker(const Component& component, const std::string& worker_name);
        
        template<class T>
        std::shared_ptr<T> GetTypedWorker(const std::string& worker_name);


        //Worker
        std::weak_ptr<Worker> AddWorker(std::unique_ptr<Worker> worker);
        std::weak_ptr<Worker> GetWorker(const std::string& worker_name);
        std::shared_ptr<Worker> RemoveWorker(const std::string& worker_name);

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
        std::mutex element_mutex_;

        std::unordered_map<std::string, std::shared_ptr<Worker> > workers_;
        std::unordered_map<std::string, std::shared_ptr<EventPort> > eventports_;    
        
        std::unordered_map<std::string, std::string> callback_functions_type_;
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

template<class T>
std::shared_ptr<T> Component::AddTypedWorker(const Component& component, const std::string& worker_name){
    static_assert(std::is_base_of<Worker, T>::value, "T must inherit from Worker");
    auto t_ptr = std::unique_ptr<T>(new T(component, worker_name));
    auto t = std::dynamic_pointer_cast<T>(AddWorker(std::move(t_ptr)).lock());
    return t;
}

template<class T>
std::shared_ptr<T> Component::GetTypedWorker(const std::string& worker_name){
    static_assert(std::is_base_of<Worker, T>::value, "T must inherit from Worker");
    return std::dynamic_pointer_cast<T>(GetWorker(worker_name).lock());
}


#endif //COMPONENT_H