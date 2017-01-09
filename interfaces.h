#ifndef INTERFACES_H
#define INTERFACES_H

#include "message.h"
#include "vectormessage.h"
#include "globalinterfaces.hpp"

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

class txVectorMessageInt : public OutEventPort<::VectorMessage>{
    public:
        virtual void txVectorMessage(::VectorMessage* message) = 0;
   // protected:
        virtual void tx_(::VectorMessage* message){};
};

class rxVectorMessageInt : public InEventPort<::VectorMessage>{
    public:
        virtual void rxVectorMessage(::VectorMessage* message) = 0;
    //protected:
        virtual void rx_(::VectorMessage* message){};
};


/*
    Concrete Component Interfaces
*/
class SenderInt: public Component, public txMessageInt, public txVectorMessageInt{
    protected:
        //Implement our 
        void txMessage(Message* message);
        void txVectorMessage(VectorMessage* message);

    public:
        SenderInt(std::string name);
        //Component Instantiation
        void activate();
        void passivate();
        
        //Attributes
        std::string instName();
        std::string message();
        void set_instName(const std::string val);
        void set_message(const std::string val);

        void _set_txMessage(txMessageInt* port);
        void _set_txVectorMessage(txVectorMessageInt* port);
    private:
        txMessageInt* txMessageInt_ = 0;
        txVectorMessageInt* txVectorMessageInt_ = 0;
        std::string instName_;
        std::string message_;
};


class ReceiverInt: public Component, public rxMessageInt, public rxVectorMessageInt{
    protected:
        //Pure virtualize our Compositions
        virtual void rxMessage(Message* message) = 0;
        virtual void rxVectorMessage(VectorMessage* message) = 0;
        
    public:
        ReceiverInt(std::string name);
        //Component Instantiation
        void activate();
        void passivate();
        
        //Attributes
        std::string instName();
        void set_instName(const std::string val);

        void _set_rxMessage(rxMessageInt* port);
        void _set_rxVectorMessage(rxVectorMessageInt* port);
    private:
        rxMessageInt* rxMessageInt_ = 0;
        rxVectorMessageInt* rxVectorMessageInt_ = 0;
        std::string instName_;
};



#endif //INTERFACES_H