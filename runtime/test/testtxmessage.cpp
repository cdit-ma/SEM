#include "testtxmessage.h"
#include <iostream>


test_txMessage::test_txMessage(txMessageInt* component, rxMessageInt* rxMessage){
    this->component_ = component;
    this->rmMessage_ = rxMessage;
}

void test_txMessage::txMessage(Message* message){
    std::cout << "test_txMessage::txMessage" << std::endl;
    rmMessage_->rxMessage(message);
}