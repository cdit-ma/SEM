#ifndef GLOBALINTERFACES_H
#define GLOBALINTERFACES_H

#include <iostream>
#include <functional>
#include <thread>

#include <deque>

#define EXPORT_FUNC __attribute__((visibility("default")))

class ActivateInt{
    private:
        bool active_ = false;
    public:
        virtual void activate(){
            this->active_ = true;
        };

        virtual void passivate(){
            this->active_ = false;
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
        void activate(){
            if(!is_active()){
                terminate = false;
                callback_thread_ = new std::thread(&PeriodicEvent::loop, this);
                ActivateInt::activate();
            }
        };
        void passivate(){
            if(is_active()){
                terminate = true;
                callback_thread_->join();
                delete callback_thread_;
                callback_thread_ = 0;
                ActivateInt::passivate();
            }
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

        void activate();
        void passivate();

        void add_event_port(EventPort* event_port);
        void remove_event_port(EventPort* event_port);
    private: 
        std::string inst_name_;
        std::deque<EventPort*> eventports_;
};


class NodeContainer: public ActivateInt{
    public:
        void activate();
        void passivate();
        virtual void startup() = 0;
        void teardown();

        void add_component(Component* component);

    private:
        std::deque<Component*> components_;
};

inline void Component::add_event_port(EventPort* event_port){
    eventports_.push_back(event_port);
};

inline void Component::remove_event_port(EventPort* event_port){
    //eventports_.push_back(event_port);
};

inline void Component::activate(){
    for(auto e : eventports_){
        e->activate();
    }
    ActivateInt::activate();
};


inline void Component::passivate(){
    for(auto e : eventports_){
        e->passivate();
    }
    ActivateInt::passivate();
};



inline Component::Component(std::string inst_name){
    inst_name_ = inst_name;
};

inline const std::string Component::get_name(){
    return inst_name_;
};

inline void NodeContainer::activate(){
    for(auto c : components_){
        std::cout << "Activating: " << c << std::endl;
        c->activate();
    }
    ActivateInt::activate();
};

inline void NodeContainer::passivate(){
    for(auto c : components_){
        std::cout << "Passivating: " << c << std::endl;
        c->passivate();
    }
    ActivateInt::passivate();
};

inline void NodeContainer::teardown(){
    while(!components_.empty()){
        delete components_.back();
        components_.pop_back();
    }
};

inline void NodeContainer::add_component(Component* component){
    components_.push_front(component);
};


#endif //GLOBALINTERFACES_H