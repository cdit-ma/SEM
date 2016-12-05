#ifndef DDSRXMESSAGE_H
#define DDSRXMESSAGE_H

#include <thread>
#include <dds/dds.hpp>


#include "../interfaces.h"
#include "message.hpp"

namespace rti{

class RxMessage: public rxMessageInt{
    public:
        RxMessage(rxMessageInt* component, dds::sub::Subscriber sub, std::string topic_name);
        void rxMessage(Message* message);
    private:
        void recieve();

        std::thread* rec_thread_;
        rxMessageInt* component_;

       //Magic DDS stuff
        dds::sub::Subscriber subscriber_ = dds::sub::Subscriber(dds::core::null);        
        dds::sub::DataReader<test_dds::Message> reader_ = dds::sub::DataReader<test_dds::Message>(dds::core::null);
};
};

#endif //DDSRXMESSAGE_H