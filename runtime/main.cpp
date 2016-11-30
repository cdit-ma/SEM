#include <iostream>
#include <chrono>
#include <thread>

#include "interfaces.h"
#include "senderimpl.h"
#include "recieverimpl.h"

#include "test/testtxmessage.h"
#include "test/testrxmessage.h"
#include "zmq/zmqrxmessage.h"
#include "zmq/zmqtxmessage.h"
#include "dds/ddstxmessage.h"
#include "dds/ddsrxmessage.h"
#include "ndds/ndds_cpp.h"
#include "dds/messageSupport.h"

#include "zmq.hpp"

void setupDDS(){
	//DDSDomainParticipantFactory* factory = DDSDomainParticipantFactory::get_instance()

    





}

int main(int argc, char** argv){


    DDSDomainParticipant *participant = DDSDomainParticipantFactory::get_instance()->
                        create_participant(
                        9,
                        DDS_PARTICIPANT_QOS_DEFAULT,
                        NULL,   /* Listener */
                        DDS_STATUS_MASK_NONE);

    DDS_PublisherQos pub_qos;
    participant->get_default_publisher_qos(pub_qos);
    DDS_SubscriberQos sub_qos;
    participant->get_default_subscriber_qos(sub_qos);
    DDS_TopicQos topic_qos;
    participant->get_default_topic_qos(topic_qos);
const char* type_name = "My_Type";
test_dds::MessageTypeSupport::register_type(participant, type_name);

                        DDSPublisher* publisher = participant->create_publisher(pub_qos, 
                                                   NULL,
                                                  DDS_STATUS_MASK_ALL);


                                                  DDSSubscriber* subscriber = 
                participant->create_subscriber(sub_qos, 
                                               NULL,
                                               DDS_STATUS_MASK_ALL);
            
            DDSTopic* topic = participant->create_topic("TOPIC", type_name,
                                      topic_qos, NULL,
                                      DDS_STATUS_MASK_ALL);

    SenderImpl* sender_impl = new SenderImpl();
    RecieverImpl* reciever_impl = new RecieverImpl();
    
    //Construct Ports
    rxMessageInt* rxMessage = new dds_rxMessage(reciever_impl, subscriber, topic);
    txMessageInt* txMessage = new dds_txMessage(sender_impl, publisher, topic);

    //zmq::context_t * context = new zmq::context_t(1);
    //txMessageInt* txMessage = new zmq_txMessage(sender_impl, context, std::string("tcp://*:6000"));

    //rxMessageInt* rxMessage = new zmq_rxMessage(reciever_impl, context, std::string("tcp://192.168.111.187:6000"));
    
    //Attach Ports
    sender_impl->txMessage_ = txMessage;
    reciever_impl->rxMessage_ = rxMessage;

    sender_impl->set_instName("SenderImpl");
    sender_impl->set_message("Hello, World!");


    int i = 60;
    while(i-- > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        sender_impl->periodic_event();
    }

    return -1;
}
