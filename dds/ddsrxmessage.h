#ifndef DDSRXMESSAGE_H
#define DDSRXMESSAGE_H

#include "../interfaces.h"

#include <thread>
#include <dds/dds.hpp>

#include "message.hpp"

class dds_rxMessage: public rxMessageInt{
    public:
        dds_rxMessage(rxMessageInt* component, dds::sub::Subscriber* subscriber, dds::topic::Topic<test_dds::Message>* topic);
        void rxMessage(Message* message);
    private:
        void recieve();


        std::thread* rec_thread_;
        rxMessageInt* component_;
        dds::sub::Subscriber* subscriber_;

        dds::sub::DataReader<test_dds::Message>* reader_;
        dds::topic::Topic<test_dds::Message>* topic_;
        
};


#endif //DDSRXMESSAGE_H