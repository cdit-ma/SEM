#include "qpidtxmessage.h"
#include <iostream>

qpid_txMessage::qpid_txMessage(txMessageInt* component, qpid::messaging::Connection* connection, std::string topic){
    this->component_ = component;
    this->connection_ = connection;
    this->session_ = connection_->createSession();
    this->sender_ = session_.createSender(topic);
}

void qpid_txMessage::txMessage(std::string message){
    qpid::messaging::Message m;
    m.setContent(message);
    sender_.send(m);
}