#ifndef COMPONENTS_HELLOSENDER_HELLOSENDERINT_H
#define COMPONENTS_HELLOSENDER_HELLOSENDERINT_H

//Add the include folder of re to CMAKEINCLUDE directory
#include <core/component.h>

//Add the Port folders to CMAKEINCLUDE directory
#include "../../datatypes/message/message.h"

class HelloSenderInt: public Component{
    public:
        HelloSenderInt(std::string name);
        
        //instName string Attribute
        std::string instName();
        void set_instName(const std::string val);

        //message string Attribute
        std::string message();
        void set_message(const std::string val);

        void txMessage(::Message* message);

        virtual void tick() = 0;
    private:
};

#endif //COMPONENTS_HELLOSENDER_HELLOSENDERINT_H
