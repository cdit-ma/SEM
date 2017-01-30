#ifndef COMPONENTS_HELLORECEIVER_HELLORECEIVERINT_H
#define COMPONENTS_HELLORECEIVER_HELLORECEIVERINT_H

//Add the Port folders to CMAKEINCLUDE directory
#include "../../datatypes/message/message.h"

//Add the include folder of re to CMAKEINCLUDE directory
#include <core/globalinterfaces.hpp>
#include <core/component.h>
#include <core/eventports/ineventport.hpp>

#include <iostream>

class HelloReceiverInt: public Component{
     public:
        HelloReceiverInt(std::string name);
        
        //instName string Attribute
        std::string instName();
        void set_instName(const std::string val);
     
        //Generic receiver function, which should call back into the Derived receive function
        void _rxMessage_(BaseMessage* m);
        virtual void rxMessage(Message* message) = 0;
        
};

#endif //COMPONENTS_HELLORECEIVER_HELLORECEIVERIMPL_H