#ifndef OUTEVENTPORT_HPP
#define OUTEVENTPORT_HPP

#include "eventport.h"
#include "../modellogger.h"
#include "../component.h"

//Interface for a standard templated OutEventPort
template <class T> class OutEventPort: public EventPort{
    public:
        OutEventPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware);
        int GetEventsReceived();
        int GetEventsSent();
        virtual bool tx(const T& t);
    private:
        std::mutex queue_mutex_;
        int tx_count = 0;
        int tx_sent_count = 0;

};

template <class T>
OutEventPort<T>::OutEventPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
:EventPort(component, port_name, EventPort::Kind::TX, middleware){
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
bool OutEventPort<T>::tx(const T& message){
    tx_count ++;
    if(is_running()){
        tx_sent_count ++;
        logger()->LogComponentEvent(*this, message, ModelLogger::ComponentEvent::SENT);
        return true;
    }else{
        logger()->LogComponentEvent(*this, message, ModelLogger::ComponentEvent::IGNORED);
    }
    return false;
};

#endif //OUTEVENTPORT_HPP
