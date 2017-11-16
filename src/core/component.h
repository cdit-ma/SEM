#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <functional>
#include <map>
#include <mutex>
#include <vector>
#include <memory>
#include "eventports/eventport.h"
#include <iostream>

#include "activatable.h"
#include "basemessage.h"

class Worker;
class EventPort;

class Component: public Activatable{
    public:
        Component(std::string inst_name ="");
        virtual ~Component();

        //EventPort
        std::weak_ptr<EventPort> AddEventPort(std::unique_ptr<EventPort> event_port);
        std::weak_ptr<EventPort> GetEventPort(const std::string& name);
        template<class T>
        std::shared_ptr<T> GetTypedEventPort(const std::string& name);
        std::shared_ptr<EventPort> RemoveEventPort(const std::string& name);
        
        //Worker
        std::weak_ptr<Worker> AddWorker(std::unique_ptr<Worker> worker);
        std::weak_ptr<Worker> GetWorker(const std::string& name);
        std::shared_ptr<Worker> RemoveWorker(const std::string& name);

        //Callback
        
        std::function<void (::BaseMessage*)> GetCallback(const std::string& name);


        template<class T>
        bool AddCallback(const std::string& name, std::function<void (T)> function);

        bool AddPeriodicCallback(const std::string& name, std::function<void()> function);


        bool RemoveCallback(const std::string& name);
    protected:
        virtual bool HandleActivate();
        virtual bool HandleConfigure();
        virtual bool HandlePassivate();
        virtual bool HandleTerminate();
    private:
        bool AddCallback_(const std::string& name, std::function<void (::BaseMessage*)> function);
        std::mutex state_mutex_;
        std::mutex mutex_;

        std::map<std::string, std::shared_ptr<Worker> > workers_;
        std::map<std::string, std::shared_ptr<EventPort> > eventports_;    
        
        std::map<std::string, std::string> callback_functions_type_;
        std::map<std::string, std::function<void (::BaseMessage*)> > callback_functions_;
};

template<class T>
std::shared_ptr<T> Component::GetTypedEventPort(const std::string& name){
    static_assert(std::is_base_of<EventPort, T>::value, "T must inherit from EventPort");
    auto p = GetEventPort(name).lock();
    return std::dynamic_pointer_cast<T>(p);
};

template<class T>
bool Component::AddCallback(const std::string& name, std::function<void (T)> function){
    static_assert(std::is_base_of<::BaseMessage, T>::value, "T must inherit from ::BaseMessage");

    return AddCallback_(name, [this, function] (::BaseMessage* message){
            auto t_message = static_cast<T*>(message);
            if(t_message){
                function(*t_message);
            }else{
                std::cerr << "Component: " << get_name() << " cannot upcast message to original type." << std::endl;
            }
        });
};


#endif //COMPONENT_H