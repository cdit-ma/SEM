#ifndef COMPONENTS_HELLORECEIVER_HELLORECEIVERINT_H
#define COMPONENTS_HELLORECEIVER_HELLORECEIVERINT_H

//Add the Port folders to CMAKEINCLUDE directory
#include "../../ports/message/message.h"

//Add the include folder of re to CMAKEINCLUDE directory
#include <core/globalinterfaces.hpp>
#include <core/component.h>
#include <core/eventports/ineventport.hpp>

class HelloReceiverInt: public Component{
     public:
        HelloReceiverInt(std::string name);
    
        //instName string Attribute
        std::string instName();
        void set_instName(const std::string val);
        
        //Setter function for the rxMessage
        void _set_rxMessage(::InEventPort<::Message>* rx);
    protected:
        //Pure virtualize the receive functions to all callback's into the Implementation
        virtual void rxMessage(::Message* message) = 0;
    private:
        ::InEventPort<::Message>* rxMessage_ = 0;
};

#endif //COMPONENTS_HELLORECEIVER_HELLORECEIVERIMPL_H