#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>


#include "interfaces.h"
#include "senderimpl.h"
#include "receiverimpl.h"

//RTI DDS
//#include "rti/rtitxmessage.h"
//#include "rti/rtirxmessage.h"

//OPENSPLICE
//#include "opensplice/osplrxmessage.h"
//#include "opensplice/ospltxmessage.h"

//ZMQ
#include "zmq/zmqrxmessage.h"
#include "zmq/zmqtxmessage.h"

//QPID
#include "qpid/qpidrxmessage.h"
#include "qpid/qpidtxmessage.h"

int main(int argc, char** argv){
    
    SenderImpl* sender_impl = new SenderImpl();
    receiverImpl* receiver_impl = new receiverImpl();

    SenderImpl* sender_impl2 = new SenderImpl();
    receiverImpl* receiver_impl2 = new receiverImpl();

    sender_impl->set_instName("RTI_SENDER");
    sender_impl2->set_instName("OSPL_SENDER");
    
    sender_impl->set_message("YO");
    sender_impl2->set_message("YO");
    
    receiver_impl->set_instName("RTI_receiveR");
    receiver_impl2->set_instName("OSPL_receiveR");
    
    std::string topic_name("Topic#1");
    std::string topic_name2("Topic#2");

    std::string pub_name("pub");
    std::string sub_name("sub");
    std::string writer_name("writer");
    std::string reader_name("reader");

    txMessageInt* rti_tx  = 0;
    rxMessageInt* rti_rx  = 0;
    txMessageInt* ospl_tx = 0;
    rxMessageInt* ospl_rx = 0;
    txMessageInt* qpid_tx = 0;
    rxMessageInt* qpid_rx = 0;
    txMessageInt* zmq_tx = 0;
    rxMessageInt* zmq_rx = 0;

    //RTI DDS
    rti_tx = new rti::TxMessage(sender_impl, 0, pub_name, topic_name);
    rti_rx = new rti::RxMessage(receiver_impl, 0, sub_name, topic_name);

    //OpenSplice DDS
    ospl_tx = new ospl::TxMessage(sender_impl2, 0, pub_name, topic_name);
    ospl_rx = new ospl::RxMessage(receiver_impl2, 0, sub_name, topic_name);

    //QPID
    //qpid_tx = new qpid::TxMessage(sender_impl, "localhost:5672", "a");
    //qpid_rx = new qpid::RxMessage(receiver_impl, "localhost:5672",  "a");
    
    //ZMQ
    zmq_tx = new zmq::TxMessage(sender_impl, std::string("tcp://*:6000"));
    zmq_rx = new zmq::RxMessage(receiver_impl, std::string("tcp://localhost:6000"));
    
    //Attach Ports
    sender_impl->_set_txMessage(rti_tx);
    receiver_impl->_set_rxMessage(rti_rx);

    sender2_impl->_set_txMessage(ospl_tx);
    receiver2_impl->_set_rxMessage(ospl_rx);


    int i = 600;
    while(i-- > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sender_impl->set_message(std::to_string(i));
        sender_impl->periodic_event();
    }

    return -1;
}
