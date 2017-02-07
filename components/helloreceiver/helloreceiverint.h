#ifndef COMPONENTS_HELLORECEIVER_HELLORECEIVERINT_H
#define COMPONENTS_HELLORECEIVER_HELLORECEIVERINT_H
//Add the include folder of re to CMAKEINCLUDE directory
#include <core/component.h>

//Add the Port folders to CMAKEINCLUDE directory
#include "../../datatypes/message/message.h"

class HelloReceiverInt: public Component{
     public:
        HelloReceiverInt(std::string name);
        
        //instName string Attribute
        std::string instName();
        void set_instName(const std::string val);
     
        virtual void rxMessage(Message* message) = 0;
};

#endif //COMPONENTS_HELLORECEIVER_HELLORECEIVERIMPL_H