#ifndef INTERFACES_H
#define INTERFACES_H

#include "message.h"
#include "vectormessage.h"

#include "common/includes/core/globalinterfaces.hpp"
#include "common/includes/core/eventports/ineventport.hpp"
#include "common/includes/core/eventports/outeventport.hpp"
#include "common/includes/core/component.h"

/*
    Concrete Component Interfaces
*/
class SenderInt: public Component{
    protected:
        //Implement our 
        void txMessage(::Message* message);
        void txVectorMessage(::VectorMessage* message);

    public:
        SenderInt(std::string name);
        
    public:
        //Attributes
        std::string instName();
        std::string message();
        void set_instName(const std::string val);
        void set_message(const std::string val);

        //Setters for our Ports
        void _set_txMessage(::OutEventPort<::Message>* tx);
        void _set_txVectorMessage(::OutEventPort<::VectorMessage>* tx);
    private:
        //Define our Ports
        ::OutEventPort<::Message>* txMessage_ = 0;
        ::OutEventPort<::VectorMessage>* txVectorMessage_ = 0;

        //Define our Attributes
        std::string instName_;
        std::string message_;
};

class ReceiverInt: public Component{
    protected:
        //Pure virtualize the receive functions to all callback's into the Implementation
        virtual void rxMessage(::Message* message) = 0;
        virtual void rxVectorMessage(::VectorMessage* message) = 0;
    public:
        ReceiverInt(std::string name);
    public:
        //Attributes
        std::string instName();
        void set_instName(const std::string val);
    
        //Setters for our Ports
        void _set_rxMessage(::InEventPort<::Message>* rx);
        void _set_rxVectorMessage(::InEventPort<::VectorMessage>* rx);
    private:
        //Define our Ports
        ::InEventPort<::Message>* rxMessage_ = 0;
        ::InEventPort<::VectorMessage>* rxVectorMessage_ = 0;
    
        //Define our Attributes
        std::string instName_;
};

#endif //INTERFACES_H