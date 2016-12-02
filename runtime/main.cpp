#include <iostream>
#include <chrono>
#include <thread>

#include "interfaces.h"
#include "senderimpl.h"
#include "recieverimpl.h"

//#include "test/testtxmessage.h"
//#include "test/testrxmessage.h"
//#include "zmq/zmqrxmessage.h"
//#include "zmq/zmqtxmessage.h"
//#include "dds/ddstxmessage.h"
//#include "dds/ddsrxmessage.h"
//#include "dds/message.hpp"
#include "opensplice/osplrxmessage.h"
#include "opensplice/ospltxmessage.h"
#include "dds/dds.hpp"
#include "opensplice/message.h"
#include "opensplice/message_DCPS.hpp"
#include "zmq.hpp"

#include <iostream>




int main(int argc, char** argv){
    dds::domain::DomainParticipant participant(0); 
    dds::pub::Publisher publisher(participant);
    dds::sub::Subscriber subscriber(participant);

    SenderImpl* sender_impl = new SenderImpl();
    //SenderImpl* sender_impl2 = new SenderImpl();
    RecieverImpl* reciever_impl = new RecieverImpl();
    
    //Construct Ports
    rxMessageInt* rxMessage = new ospl_rxMessage(reciever_impl, subscriber, "Test");
    txMessageInt* txMessage = new ospl_txMessage(sender_impl, publisher, "Test");

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
