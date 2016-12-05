#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>


#include "interfaces.h"
#include "senderimpl.h"
#include "recieverimpl.h"

//#include "test/testtxmessage.h"
//#include "test/testrxmessage.h"

//#include "zmq.hpp"
//#include "zmq/zmqrxmessage.h"
//#include "zmq/zmqtxmessage.h"


#include <dds/dds.hpp>

//RTI DDS
#include "rti/rtitxmessage.h"

#include "rti/rtirxmessage.h"

//OPENSPLICE
//#include "opensplice/osplrxmessage.h"
//#include "opensplice/ospltxmessage.h"


int main(int argc, char** argv){
    dds::domain::DomainParticipant participant(0); 
    dds::pub::Publisher publisher(participant);
    dds::sub::Subscriber subscriber(participant);


    SenderImpl* sender_impl = new SenderImpl();
    RecieverImpl* reciever_impl = new RecieverImpl();
    
    //Construct Ports
    std::string topic("TEST");
     //new rti::TxMessage(sender_impl, publisher, topic);
    txMessageInt* txMessage = new rti::TxMessage(sender_impl, publisher, "Test2");
    rxMessageInt* rxMessage = new rti::RxMessage(reciever_impl, subscriber, "Test2");

    //txMessageInt* txMessage = new ospl::TxMessage(sender_impl, publisher, "Test2");
    //rxMessageInt* rxMessage = new ospl::RxMessage(reciever_impl, subscriber, "Test2");

    //ZMQ
    //zmq::context_t * context = new zmq::context_t(1);
    //txMessageInt* txMessage = new zmq_txMessage(sender_impl, context, std::string("tcp://*:6000"));
    //rxMessageInt* rxMessage = new zmq_rxMessage(reciever_impl, context, std::string("tcp://192.168.111.187:6000"));
    
    
    //Attach Ports
    sender_impl->txMessage_ = txMessage;
    reciever_impl->rxMessage_ = rxMessage;

    sender_impl->set_instName("SenderImpl");
    sender_impl->set_message("Hello, World!");
    
    //sender_impl2->txMessage_ = txMessage2;
    //sender_impl2->set_instName("SenderImpl2");
    //sender_impl2->set_message("Hello, World2!");


    int i = 600;
    while(i-- > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        sender_impl->periodic_event();
        //sender_impl2->periodic_event();
    }

    return -1;
}
