#include "qpidrxmessage.h"
#include "qpidhelper.h"

#include <iostream>

#include "qpid/messaging/Connection.h"
#include "qpid/messaging/Message.h"
#include "qpid/messaging/Receiver.h"
#include "qpid/messaging/Sender.h"
#include "qpid/messaging/Session.h"

qpid_rxMessage::qpid_rxMessage(rxMessageInt* component, std::string broker, std::string topic){
    this->component_ = component;
    broker_ = broker;

    this->topic_ = topic;

    rec_thread_ = new std::thread(&qpid_rxMessage::recieve, this);
}

void qpid_rxMessage::rxMessage(Message* message){
    component_->rxMessage(message);
}

void qpid_rxMessage::recieve(){
    qpid::QpidHelper* helper = qpid::QpidHelper::get_qpid_helper();
    auto connection = helper->get_connection(broker_);
    connection->open();
    std::cout << "RX" << std::endl;
    std::cout << broker_ << std::endl;
    auto session = connection->createSession();
        std::cout << "HERE" << std::endl;
    auto receiver = session.createReceiver(topic_);
    
    while(true){
        //Wait for next message
        qpid::messaging::Message message = receiver.fetch();
        session.acknowledge();
        /*
        //If we have a valid message
        if(data->size() > 0){
            //Construct a string out of the zmq data
            std::string msg_str(static_cast<char *>(data->data()), data->size());
            
            //Construct a Protobuf object
            proto::Message* message = new proto::Message();

            if (message->ParseFromString(msg_str)){
                Message* modelMessage = proto::proto_to_message(message);
                this->rxMessage(modelMessage);
            }
        }*/
        std::cout << message.getContent() << std::endl;
    }

}