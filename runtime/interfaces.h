#ifndef INTERFACES_H
#define INTERFACES_H

#include "message.h"
#include "globalinterfaces.h"

/*
    Port Interfaces
*/

class txMessageInt{
    public:
        virtual void txMessage(Message* message) = 0;
};

class rxMessageInt : public InEventPort{
    public:
        virtual void rxMessage(Message* message) = 0;
        virtual void notify(){};
};

class SenderInt: public txMessageInt{
    protected:
        //Pure virtualize our Compositions
        void txMessage(Message* message);

    public:
        //Attributes
        std::string instName();
        std::string message();
        void set_instName(const std::string val);
        void set_message(const std::string val);

        void _set_txMessage(txMessageInt* port);
    private:

        txMessageInt* txMessageInt_ = 0;
        std::string instName_;
        std::string message_;
};



class RecieverInt: public rxMessageInt{
    protected:
        //Pure virtualize our Compositions
        virtual void rxMessage(Message* message) = 0;
        
    public:
        //Attributes
        std::string instName();
        void set_instName(const std::string val);

        void _set_rxMessage(rxMessageInt* port);
    private:
        rxMessageInt* rxMessageInt_ = 0;
        std::string instName_;
};



#endif //INTERFACES_H