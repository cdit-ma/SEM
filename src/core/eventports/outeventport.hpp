#ifndef OUTEVENTPORT_HPP
#define OUTEVENTPORT_HPP

#include "../modellogger.h"
#include "../component.h"
#include "../eventport.h"

//Interface for a standard templated OutEventPort
template <class T> class OutEventPort: public EventPort{
    public:
        OutEventPort(Component* component, std::string name);
        virtual ~OutEventPort();
        virtual bool Activate();
        virtual bool Passivate();
        virtual bool Teardown();
        
        virtual void tx(T* t);
};

template <class T>
OutEventPort<T>::OutEventPort(Component* component, std::string name)
:EventPort(component, name, EventPort::Kind::TX){
};

template <class T>
OutEventPort<T>::~OutEventPort(){
    //Teardown
};

template <class T>
void OutEventPort<T>::tx(T* t){
    if(is_active() && logger()){
        logger()->LogComponentEvent(this, t, ModelLogger::ComponentEvent::SENT);
    }
};

template <class T>
bool OutEventPort<T>::Activate(){
    return EventPort::Activate();
};

template <class T>
bool OutEventPort<T>::Passivate(){
    return EventPort::Passivate();
};


template <class T>
bool OutEventPort<T>::Teardown(){
    return EventPort::Teardown();
};


#endif //OUTEVENTPORT_HPP
