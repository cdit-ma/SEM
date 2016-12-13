#include "qpidtxmessage.h"
#include <iostream>
#include "qpidhelper.h"

#include "qpid/messaging/Connection.h"
#include "qpid/messaging/Message.h"
#include "qpid/messaging/Receiver.h"
#include "qpid/messaging/Sender.h"
#include "qpid/messaging/Session.h"

qpid_txMessage::qpid_txMessage(txMessageInt* component, std::string broker, std::string topic){

    this->component_ = component;
    broker_ = broker;
    topic_ = topic;
}

void qpid_txMessage::txMessage(Message* message){
    qpid::QpidHelper* helper = qpid::QpidHelper::get_qpid_helper();
    auto connection = helper->get_connection(broker_);
    std::cout <<"tx" << std::endl;
    auto session = connection->createSession();
    auto sender = session.createSender(topic_);
    qpid::messaging::Message m;
    m.setContent("Hi");
    sender.send(m);
    connection->close();
}