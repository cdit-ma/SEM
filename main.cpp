#include <iostream>
#include <chrono>
#include <thread>

#include "interfaces.h"
#include "senderimpl.h"
#include "recieverimpl.h"

#include "test/testtxmessage.h"
#include "test/testrxmessage.h"

int main(int argc, char** argv){
    SenderImpl* sender_impl = new SenderImpl();
    RecieverImpl* reciever_impl = new RecieverImpl();
    
    //Construct Ports
    rxMessageInt* rxMessage = new test_rxMessage(reciever_impl);
    txMessageInt* txMessage = new test_txMessage(sender_impl, rxMessage);

    //Attach Ports
    sender_impl->txMessage_ = txMessage;
    reciever_impl->rxMessage_ = rxMessage;



    int i = 60;
    while(i-- > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        sender_impl->periodic_event();
    }

    return -1;
}
