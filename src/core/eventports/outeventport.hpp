#ifndef OUTEVENTPORT_HPP
#define OUTEVENTPORT_HPP

#include "../modellogger.h"
#include "../component.h"
#include "../eventport.h"

//Interface for a standard templated OutEventPort
template <class T> class OutEventPort: public EventPort{
    public:
        OutEventPort(Component* component, std::string name, std::string middleware);
        virtual bool Activate();
        virtual bool Passivate();
        virtual bool Teardown();
        
        virtual void tx(T* t);
};

template <class T>
OutEventPort<T>::OutEventPort(Component* component, std::string name, std::string middleware)
:EventPort(component, name, EventPort::Kind::TX, middleware){
};


template <class T>
void OutEventPort<T>::tx(T* t){
    if(is_active() && logger()){
        logger()->LogComponentEvent(this, t, ModelLogger::ComponentEvent::SENT);
    }
};

template <class T>
bool OutEventPort<T>::Activate(){
    auto success = EventPort::Activate();
    if(success){
        EventPort::LogActivation();
    }
    return success;
};

template <class T>
bool OutEventPort<T>::Passivate(){
    auto success = EventPort::Passivate();
    if(success){
        EventPort::LogPassivation();
    }
    return success;
};


template <class T>
bool OutEventPort<T>::Teardown(){
    return EventPort::Teardown();
};


#endif //OUTEVENTPORT_HPP
