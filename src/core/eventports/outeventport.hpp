#ifndef OUTEVENTPORT_HPP
#define OUTEVENTPORT_HPP

#include "eventport.h"
#include "../modellogger.h"
#include "../component.h"

//Interface for a standard templated OutEventPort
template <class T> class OutEventPort: public EventPort{
    public:
        OutEventPort(std::shared_ptr<Component> component, std::string name, std::string middleware);
        int GetEventsReceived();
        int GetEventsSent();
    protected:
        bool tx(T* t);
    private:
        std::mutex queue_mutex_;
        int tx_count = 0;
        int tx_sent_count = 0;

};

template <class T>
OutEventPort<T>::OutEventPort(std::shared_ptr<Component> component, std::string name, std::string middleware)
:EventPort(component, name, EventPort::Kind::TX, middleware){
};
template <class T>
int OutEventPort<T>::GetEventsReceived(){
    return tx_count;    
}

template <class T>
int OutEventPort<T>::GetEventsSent(){
    return tx_sent_count;    
}


template <class T>
bool OutEventPort<T>::tx(T* t){
    if(t){
        tx_count ++;
        if(is_running()){
            tx_sent_count ++;
            //logger()->LogComponentEvent(*this, t, ModelLogger::ComponentEvent::SENT);
            return true;
        }else{
            //logger()->LogComponentEvent(*this, t, ModelLogger::ComponentEvent::IGNORED);
        }
    }
    return false;
};
/*
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
};*/


#endif //OUTEVENTPORT_HPP
