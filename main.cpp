#include <iostream>
#include "interfaces.h"
#include "senderimpl.h"

#include "test/testtxmessage.h"
#include "test/testrxmessage.h"

int main(int argc, char** argv){
    

    SenderImpl* sender_impl = new SenderImpl();
    sender_impl->txMessage_ = new test_txMessage();
    
    sender_impl->periodic_event();
    return -1;
}
