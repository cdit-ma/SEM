#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <functional>
#include <map>
#include <mutex>
#include <vector>


#include "activatable.h"
#include "basemessage.h"

class Worker;
class EventPort;

class Component: public Activatable{
    public:
        Component(std::string inst_name ="");
        virtual ~Component();
        bool Activate();
        bool Passivate();
        bool Teardown();

        void AddWorker(std::shared_ptr<Worker> worker);

        std::shared_ptr<Worker> GetWorker(std::string name);

        void AddEventPort(std::shared_ptr<EventPort> event_port);
        void RemoveEventPort(std::shared_ptr<EventPort> event_port);
        std::shared_ptr<EventPort> GetEventPort(std::string name);

        void AddCallback(std::string port_name, std::function<void (::BaseMessage*)> function);
        std::function<void (::BaseMessage*)> GetCallback(std::string port_name);
    private:
        std::mutex state_mutex_;
        std::mutex mutex_;

        
        std::map<std::string, std::shared_ptr<Worker> > workers_;
        std::map<std::string, std::shared_ptr<EventPort> > eventports_;    
        std::map<std::string, std::function<void (::BaseMessage*)> > callback_functions_;
        std::string inst_name_;
};

#endif //COMPONENT_H