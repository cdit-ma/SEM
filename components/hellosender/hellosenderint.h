#ifndef COMPONENTS_HELLOSENDER_HELLOSENDERINT_H
#define COMPONENTS_HELLOSENDER_HELLOSENDERINT_H

//Add the Port folders to CMAKEINCLUDE directory
#include "../../ports/message/message.h"

//Add the include folder of re to CMAKEINCLUDE directory
#include "core/globalinterfaces.hpp"
#include "core/component.h"
#include "core/eventports/outeventport.hpp"

class HelloSenderInt: public Component{
    public:
        HelloSenderInt(std::string name);
        
        //instName string Attribute
        std::string instName();
        void set_instName(const std::string val);

        //message string Attribute
        std::string message();
        void set_message(const std::string val);

        //setter function for the txMessage
        void _set_txMessage(::OutEventPort<::Message>* tx);
    protected:
        void txMessage(::Message* message);
    private:
        ::OutEventPort<::Message>* txMessage_ = 0;
};

#endif //COMPONENTS_HELLOSENDER_HELLOSENDERINT_H