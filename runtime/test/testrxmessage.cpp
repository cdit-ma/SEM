#include "testrxmessage.h"
#include <iostream>


test_rxMessage::test_rxMessage(rxMessageInt* c){
    this->component_ = c;
}

void test_rxMessage::rxMessage(Message* message){
    std::cout << "test_rxMessage::rxMessage" << std::endl;
    this->component_->rxMessage(message);
}