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



//RTI DDS
#include "rti/rtitxmessage.h"
#include "rti/rtirxmessage.h"

//OPENSPLICE
#include "opensplice/osplrxmessage.h"
#include "opensplice/ospltxmessage.h"


int main(int argc, char** argv){
    
    SenderImpl* sender_impl = new SenderImpl();
    RecieverImpl* reciever_impl = new RecieverImpl();

    SenderImpl* sender_impl2 = new SenderImpl();
    RecieverImpl* reciever_impl2 = new RecieverImpl();
    
    
    std::string topic_name("TEST");
    std::string topic_name2("TEST2");

    std::string pub_name("pub");
    std::string sub_name("sub");
    std::string writer_name("writer");
    std::string reader_name("reader");

    std::string pub_name2("pub2");
    std::string sub_name2("sub2");
    std::string writer_name2("writer2");
    std::string reader_name2("reader2");
    

    rti::TxMessage* txMessage_r = new rti::TxMessage(sender_impl, 0, pub_name, writer_name, topic_name);
    //rti::RxMessage* rxMessage_r = new rti::RxMessage(reciever_impl, 0, sub_name, reader_name, topic_name);

    //ospl::TxMessage* txMessage_o = new ospl::TxMessage(sender_impl2, 0, pub_name2, writer_name2, topic_name2);
    ospl::RxMessage* rxMessage_o = new ospl::RxMessage(reciever_impl2, 0, sub_name2, reader_name2, topic_name);

    //ZMQ
    //zmq::context_t * context = new zmq::context_t(1);
    //txMessageInt* txMessage = new zmq_txMessage(sender_impl, context, std::string("tcp://*:6000"));
    //rxMessageInt* rxMessage = new zmq_rxMessage(reciever_impl, context, std::string("tcp://192.168.111.187:6000"));
    
    
    //Attach Ports
    sender_impl->txMessage_ = txMessage_r;
    //reciever_impl->rxMessage_ = rxMessage_r;

    sender_impl->set_instName("tx_rti");
    //sender_impl->set_message("1");
    
    //sender_impl2->txMessage_ = txMessage_o;
    reciever_impl2->rxMessage_ = rxMessage_o;

    //sender_impl2->set_instName("rx_ospl");
    //sender_impl2->set_message("Hello, World2!");

    reciever_impl->set_instName("rx_rti");
    reciever_impl2->set_instName("rx_ospl");
    



    

    int i = 600;
    while(i-- > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        /* Message* msg = new Message();
         Message* msg2 = new Message();
         msg->set_time(i);
            msg2->set_time(i);
        //txMessage_r->txMessage(msg);
        //txMessage_r->txMessage(msg);
        txMessage_r->txMessage(msg);
        txMessage_o->txMessage(msg2);*/

        //std::cout << "Waiting for message" << std::endl;
        sender_impl->periodic_event();
        //sender_impl2->periodic_event();
    }

    return -1;
}
