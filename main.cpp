#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>


#include "interfaces.h"
#include "senderimpl.h"
#include "receiverimpl.h"

//RTI DDS
#include "rti/rtitxmessage.h"
#include "rti/rtirxmessage.h"

//OPENSPLICE
#include "opensplice/osplrxmessage.h"
#include "opensplice/ospltxmessage.h"

//ZMQ
#include "zmq/zmqrxmessage.h"
#include "zmq/zmqtxmessage.h"
#include "zmq/zmqtxvectormessage.h"
#include "zmq/zmqrxvectormessage.h"

//QPID
#include "qpid/qpidrxmessage.h"
#include "qpid/qpidtxmessage.h"
#include "qpid/qpidrxvectormessage.h"
#include "qpid/qpidtxvectormessage.h"

class NodeContainerInstance: public NodeContainer{
    public:
    void startup(){
        SenderImpl* sender_impl = new SenderImpl("sender_impl");
        SenderImpl* sender_impl2 = new SenderImpl("sender_impl2");
        SenderImpl* sender_impl3 = new SenderImpl("sender_impl3");
        SenderImpl* sender_impl4 = new SenderImpl("sender_impl4");

        ReceiverImpl* receiver_impl = new ReceiverImpl("receiver_impl");
        ReceiverImpl* receiver_impl2 = new ReceiverImpl("receiver_impl2");
        ReceiverImpl* receiver_impl3 = new ReceiverImpl("receiver_impl3");
        ReceiverImpl* receiver_impl4 = new ReceiverImpl("receiver_impl4");


        //Add Periodic Event
        PeriodicEvent* pe = new PeriodicEvent(std::function<void(void)>(std::bind(&SenderImpl::periodic_event, sender_impl3)), 1000);
        sender_impl3->add_event_port(pe);
        
        
        sender_impl->set_instName("RTI_SENDER");
        sender_impl2->set_instName("OSPL_SENDER");
        sender_impl3->set_instName("ZMQ_SENDER");
        sender_impl4->set_instName("QPID_SENDER");
        
        sender_impl->set_message("RTI");
        sender_impl2->set_message("OSPL");
        sender_impl3->set_message("ZMQ");
        sender_impl4->set_message("QPID");
        
        receiver_impl->set_instName("RTI_Receiver");
        receiver_impl2->set_instName("OSPL_Receiver");
        receiver_impl3->set_instName("ZMQ_Receiver");
        receiver_impl4->set_instName("QPID_Receiver");
        
        std::string topic_name("Topic1");
        std::string topic_name2("Topic2");

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

        txVectorMessageInt* zmq_v_tx = 0;
        rxVectorMessageInt* zmq_v_rx = 0;

        txVectorMessageInt* qpid_v_tx = 0;
        rxVectorMessageInt* qpid_v_rx = 0;

        //RTI DDS
        //rti_tx = new rti::TxMessage(sender_impl, 0, pub_name, topic_name);
        //rti_rx = new rti::RxMessage(receiver_impl, 0, sub_name, topic_name);

        //OpenSplice DDS
        //ospl_tx = new ospl::TxMessage(sender_impl2, 0, pub_name, topic_name);
        //ospl_rx = new ospl::RxMessage(receiver_impl2, 0, sub_name, topic_name);

        //ZMQ
        zmq_tx = new zmq::TxMessage(sender_impl3, std::string("tcp://*:6000"));
        zmq_rx = new zmq::RxMessage(receiver_impl3, std::string("tcp://localhost:6000"));
        zmq_v_tx = new zmq::TxVectorMessage(sender_impl3, std::string("tcp://*:6001"));
        zmq_v_rx = new zmq::RxVectorMessage(receiver_impl3, std::string("tcp://localhost:6001"));
        //QPID
        //qpid_tx = new qpid::TxMessage(sender_impl4, "localhost:5672", "a");
        //qpid_rx = new qpid::RxMessage(receiver_impl4, "localhost:5672",  "a");
        
        //qpid_v_tx = new qpid::TxVectorMessage(sender_impl4, "localhost:5672", "b");
        //qpid_v_rx = new qpid::RxVectorMessage(receiver_impl4, "localhost:5672",  "b");

        
        //Attach Ports
        sender_impl->_set_txMessage(rti_tx);
        sender_impl2->_set_txMessage(ospl_tx);
        sender_impl3->_set_txMessage(zmq_tx);
        sender_impl3->_set_txVectorMessage(zmq_v_tx);

        
        sender_impl4->_set_txMessage(qpid_tx);
        sender_impl4->_set_txVectorMessage(qpid_v_tx);

        receiver_impl->_set_rxMessage(rti_rx);
        receiver_impl2->_set_rxMessage(ospl_rx);
        receiver_impl3->_set_rxMessage(zmq_rx);
        receiver_impl3->_set_rxVectorMessage(zmq_v_rx);
        receiver_impl4->_set_rxMessage(qpid_rx);
        receiver_impl4->_set_rxVectorMessage(qpid_v_rx);
        
        add_component(sender_impl);
        add_component(sender_impl2);
        add_component(sender_impl3);
        add_component(sender_impl4);
        add_component(receiver_impl);
        add_component(receiver_impl2);
        add_component(receiver_impl3);
        add_component(receiver_impl4);
    };
};


extern "C" NodeContainer* create_object()
{
  return new NodeContainerInstance();
}

extern "C" void destroy_object(NodeContainer* object)
{ 
    NodeContainerInstance* instance = static_cast<NodeContainerInstance*>(object);
    if(instance){
        std::cout << "Deleting Instance: " << object << std::endl;
        delete instance;
    }
}