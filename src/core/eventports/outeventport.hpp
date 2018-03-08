#ifndef OUTEVENTPORT_HPP
#define OUTEVENTPORT_HPP

#include "eventport.h"
#include "../modellogger.h"
#include "../component.h"
#include "translator.h"

//Interface for a standard templated OutEventPort
template <class T> class OutEventPort: public EventPort{
    public:
        OutEventPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware);
        virtual bool tx(const T& t);
};

template <class T>
OutEventPort<T>::OutEventPort(std::weak_ptr<Component> component, const std::string& port_name, const std::string& middleware)
:EventPort(component, port_name, EventPort::Kind::TX, middleware){
};

template <class T>
bool OutEventPort<T>::tx(const T& message){
    //Log the recieving
    EventRecieved(message);

    bool sent_message = is_running();
    
    if(sent_message){
        logger()->LogComponentEvent(*this, message, ModelLogger::ComponentEvent::SENT);
    }
    
    EventProcessed(message, sent_message);
    return sent_message;
};

#endif //OUTEVENTPORT_HPP
