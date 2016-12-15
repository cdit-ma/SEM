#ifndef INTERFACES_H
#define INTERFACES_H

#include "message.h"
#include "globalinterfaces.h"

/*
    Concrete Port Interfaces
*/

class txMessageInt : public OutEventPort<::Message>{
    public:
        virtual void txMessage(::Message* message) = 0;
   // protected:
        virtual void tx_(::Message* message){};
};

class rxMessageInt : public InEventPort<::Message>{
    public:
        virtual void rxMessage(::Message* message) = 0;
    //protected:
        virtual void rx_(::Message* message){};
};


/*
    Concrete Component Interfaces
*/


class SenderInt: public txMessageInt{
    protected:
        //Implement our 
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


class ReceiverInt: public rxMessageInt{
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