#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>


#include "interfaces.h"
#include "senderimpl.h"
#include "recieverimpl.h"

//RTI DDS
//#include "rti/rtitxmessage.h"
//#include "rti/rtirxmessage.h"

//OPENSPLICE
//#include "opensplice/osplrxmessage.h"
//#include "opensplice/ospltxmessage.h"

//ZMQ
//#include "zmq/zmqrxmessage.h"
//#include "zmq/zmqtxmessage.h"

//QPID
#include "qpid/qpidrxmessage.h"
#include "qpid/qpidtxmessage.h"

int main(int argc, char** argv){
    
    SenderImpl* sender_impl = new SenderImpl();
    RecieverImpl* reciever_impl = new RecieverImpl();

    SenderImpl* sender_impl2 = new SenderImpl();
    RecieverImpl* reciever_impl2 = new RecieverImpl();

    sender_impl2->set_instName("tx_ospl");
    sender_impl2->set_message("2");
    
    reciever_impl->set_instName("rx_rti");
    reciever_impl2->set_instName("rx_ospl");
    
    
    std::string topic_name("bye");
    std::string topic_name2("TEST232");

    std::string pub_name("pub");
    std::string sub_name("sub");
    std::string writer_name("writer");
    std::string reader_name("reader");

    std::string pub_name2("pub2");
    std::string sub_name2("sub2");
    std::string writer_name2("writer2");
    std::string reader_name2("reader2");
    
/*  
    txMessageInt* rti_tx  = 0;
    rxMessageInt* rti_rx  = 0;
    txMessageInt* ospl_tx = 0;
    rxMessageInt* ospl_rx = 0;
*/
    txMessageInt* qpid_tx = 0;
    rxMessageInt* qpid_rx = 0;



    qpid_tx = new qpid::TxMessage(sender_impl, "localhost:5672", "a");
    qpid_rx = new qpid::RxMessage(reciever_impl, "localhost:5672",  "a");

    //rti_tx = new rti::TxMessage(sender_impl, 0, pub_name, writer_name, topic_name);
    //rti_rx = new rti::RxMessage(reciever_impl, 0, sub_name, reader_name, topic_name);

    //ospl_tx = new ospl::TxMessage(sender_impl, 0, pub_name, writer_name, topic_name);
    //ospl_tx = new ospl::TxMessage(sender_impl2, 0, pub_name, writer_name2, topic_name2);
    //ospl_rx = new ospl::RxMessage(reciever_impl2, 0, sub_name, reader_name, topic_name2);


    //txMessageInt* ospl_tx2  = new ospl::TxMessage(sender_impl2, 1, pub_name, writer_name, topic_name2);

    //ZMQ
    //ospl_tx = new zmq::TxMessage(sender_impl, std::string("tcp://*:6000"));
    //ospl_rx = new zmq::RxMessage(reciever_impl, std::string("tcp://192.168.111.83:6000"));
    
    
    sender_impl->set_instName("tx_rti");

    //sender_impl2->set_instName("tx_ospl");
    //sender_impl2->set_message("2");

    //Attach Ports
    sender_impl->_set_txMessage(qpid_tx);
    reciever_impl->_set_rxMessage(qpid_rx);
    
    //sender_impl2->_set_txMessage(ospl_tx);
    //reciever_impl2->_set_rxMessage(ospl_rx);
    

    int i = 600;
    while(i-- > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sender_impl->set_message(std::to_string(i));
        sender_impl->periodic_event();
        std::cout << std::endl;
    }

    return -1;
}
