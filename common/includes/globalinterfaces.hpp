#ifndef GLOBALINTERFACES_H
#define GLOBALINTERFACES_H

#include <iostream>
#include <functional>
#include <thread>

#include <deque>
#include <unordered_map>

#define EXPORT_FUNC __attribute__((visibility("default")))

class ActivateInt{
    private:
        bool active_ = false;
    public:
        virtual bool activate(){
            this->active_ = true;
            return true;          
        };

        virtual bool passivate(){
            this->active_ = false;
            return true;
        };

        bool is_active(){
            return this->active_;
        };
};

//Interface for a standard In Event Port

class EventPort: public ActivateInt{
};

class PeriodicEvent: public EventPort{
    public:
        PeriodicEvent(std::function<void(void)> callback, int milliseconds){
            this->milliseconds_ = milliseconds;
            callback_thread_ = 0;
            this->callback_ = callback;
        }
        bool activate(){
            if(!is_active()){
                terminate = false;
                callback_thread_ = new std::thread(&PeriodicEvent::loop, this);
                return ActivateInt::activate();
            }
            return true;
        };
        bool passivate(){
            if(is_active()){
                terminate = true;
                callback_thread_->join();
                delete callback_thread_;
                callback_thread_ = 0;
                return ActivateInt::passivate();
            }
            return true;
        };

    private:
        void loop(){
            while(!terminate){
                if(callback_ != nullptr){
                    callback_();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_));
            }
        };
        bool terminate = false;
        std::function<void(void)> callback_ ;
        std::thread * callback_thread_;
        int milliseconds_;
};

template <class T> class InEventPort: public EventPort{
    public:
        virtual void rx_(T*) = 0;
};



//Interface for a standard Out Event Port
template <class T> class OutEventPort: public EventPort{
    public:
        virtual void tx_(T*) = 0;
};

class Component: public ActivateInt{
    public:
        Component(std::string inst_name);

        const std::string get_name();

        bool activate();
        bool passivate();

        void add_event_port(EventPort* event_port);
        void remove_event_port(EventPort* event_port);
    private: 
        std::string inst_name_;
        std::deque<EventPort*> eventports_;
};


class NodeContainer{
    public:
        bool activate(std::string component_name);
        bool passivate(std::string component_name);
        
        bool activate_all();
        bool passivate_all();

        virtual void startup() = 0;
        void teardown();

        bool add_component(Component* component);
        Component* get_component(std::string component_name);

    private:
        std::unordered_map<std::string, Component*> components_;
};

inline void Component::add_event_port(EventPort* event_port){
    eventports_.push_back(event_port);
};

inline void Component::remove_event_port(EventPort* event_port){
    //eventports_.push_back(event_port);
};

inline bool Component::activate(){
    for(auto e : eventports_){
           e->activate();
    }
    ActivateInt::activate();
    return true;
};

inline bool Component::passivate(){
    for(auto e : eventports_){
        e->passivate();
    }
    ActivateInt::passivate();
    return true;
};



inline Component::Component(std::string inst_name){
    inst_name_ = inst_name;
};

inline const std::string Component::get_name(){
    return inst_name_;
};

inline bool NodeContainer::activate_all(){
    for(auto c : components_){
        std::cout << "Activating: " << c.second << std::endl;
        c.second->activate();
    }
    return true;
};

inline bool NodeContainer::activate(std::string component_name){
    Component* component = get_component(component_name);
    if(component){
        return component->activate();
    }
    return false;
};

inline bool NodeContainer::passivate(std::string component_name){
    Component* component = get_component(component_name);
    if(component){
        return component->passivate();
    }
    return false;
};

inline bool NodeContainer::passivate_all(){
    for(auto c : components_){
        std::cout << "Passivating: " << c.second << std::endl;
        c.second->passivate();
    }
    return true;
};

inline void NodeContainer::teardown(){

    for (auto c : components_) {
        components_.erase(c.first);
    }
};

inline bool NodeContainer::add_component(Component* component){
    std::string component_name = component->get_name();

    //Search pub_lookup_ for key
    auto search = components_.find(component_name);
    
    if(search == components_.end()){
        std::pair<std::string, Component*> insert_pair(component_name, component);
        //Insert into hash
        components_.insert(insert_pair);
        return true;
    }else{
        std::cout << " NOT A UNIQUE NAME!" << std::endl;
        return false;
    }
};

inline Component* NodeContainer::get_component(std::string component_name){
    //Search pub_lookup_ for key
    auto search = components_.find(component_name);
    
    if(search == components_.end()){
        return 0;
    }else{
        return search->second;
    }
};




#endif //GLOBALINTERFACES_H