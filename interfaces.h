#ifndef INTERFACES_H
#define INTERFACES_H

#include "message.h"

/*
    Port Interfaces
*/

class txMessageInt{
    public:
    virtual void txMessage(Message* message) = 0;
};

class rxMessageInt{
    public:
    virtual void rxMessage(Message* message) = 0;
};

class SenderInt: public txMessageInt{
    public:
        //Pure virtualize our Compositions
        virtual void txMessage(Message* message) = 0;

        //Attributes
        std::string instName();
        void set_instName(const std::string val);
    private:
        std::string instName_;
};



class RecieverInt: public rxMessageInt{
    public:
        //Pure virtualize our Compositions
        virtual void rxMessage(Message* message) = 0;

        //Attributes
        std::string instName();
        void set_instName(const std::string val);
    private:
        std::string instName_;
};



#endif //INTERFACES_H