struct MSG{
    enum MESSAGE_TYPE; //FOR UPCASTING back to concrete Message Objects
}

struct Message : MSG{
    std::string text;
    std::vector<int> values;
};

main(){
    DDS_Participant* p = 0;

    txMessageInterface* writer = new txMessage_RTIDDS(p);
    rxMessageInterface* reader = new rxMessage_ZMQ("PORTS");

    ComponentInterface* interface = new ComponentImpl();
    interface->setTxMessage(writer);
    interface->setRxMessage(reader);
    //Set the middleware specific gear.
}

//Port interfaces
class txMessageInterface{
    virtual void txMessage(Message* message) = 0;
}

class rxMessageInterface{
    virtual void rxMessage(Message* message) = 0;
} 


//Concrete middleware implementations
class txMessage_RTIDDS: public txMessageInterface{
    public:
        void txMessage(Message* message);
    private:
        RTIDDS_message_t * messageToMW(Message* message);
}

class rxMessage_ZMQ: public rxMessageInterface{
    public:
        void rxMessage(Message* message);
    private:
        ZMQ_message_t* messageToMW(Message* message);
}

//Multiple inheritance
class ComponentInterface: public txMessageInterface, public rxMessageInterface{
    public:
        //Pure virtualize our Compositions
        virtual void txMessage(Message* message) = 0;
        virtual void rxMessage(Message* message) = 0;

        //Attributes
        std::string attribute();
        void setAttribute(std::string val);
}

//Middleware Layer Component definition
class ComponentMW : ComponentInterface{
    public:
        ComponentMW(){
            //Middleware specific instructions
            this->txMessage_ = new txMessage_RTIDDS();
            //Bind the rxMessage_ end point to this->rxMessage();
            this->rxMessage_ = new rxMessage_ZMQ();
        }

        //Recievers should be pure virtual so they are implemented by the impl.
        virtual void rxMessage(Message* message) = 0;

        void txMessage(Message* message)
        {
            //Log message
            logMessage(message);
            //Send it via the concrete middleware
            this->txMessage_->txMessage(message);
        }

    private:
        txMessageInterface* txMessage_;
        rxMessageInterface* rxMessage_;
}



class ComponentImpl: ComponentMWInterface{

    //Override only the recieving messages
    void rxMessage(Message* message){
        //Business Logic yo!

        //Call up into the middleware layer
        txMessage(message);
    }
}