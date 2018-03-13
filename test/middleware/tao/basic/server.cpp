#include <iostream>
#include <future>
#include <thread>
#include <signal.h>

#include <middleware/tao/helper.h>
#include <nodemanager/execution.hpp>

#include "server.h"


Execution* exe = 0;

void signal_handler(int sig)
{
    exe->Interrupt();
}

void print_message(const Test::Message& message){
    std::cout << "\ttime: " << message.time << std::endl;
    std::cout << "\tinst_name: " << message.inst_name << std::endl;
    std::cout << "\tBlocking" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "\tDone" << std::endl;
}


void Hello::send(const Test::Message& message){
    std::cout << "GOT send: " << std::endl;
    print_message(message);
}

void Hello::send22(const Test::Message& message){
    std::cout << "GOT send22: " << std::endl;
    print_message(message);
}

int main(int argc, char ** argv){
    signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
    exe = new Execution();
    auto helper = tao::TaoHelper::get_tao_helper();
    auto orb = helper->get_orb("iiop://192.168.111.90:50007");

    if(!orb){
        std::cerr << "DED RAT" << std::endl;
        return -1;
    }
    
    
    // Create the child POA for the test logger factory servants.
    auto sender1_poa = helper->get_poa(orb, "Sender1");
    auto sender2_poa = helper->get_poa(orb, "Sender2");
    
    //Construct a sender
    auto sender1_impl = new Hello();
    auto sender2_impl = new Hello();

    //Activate WITH ID
    //Convert our string into an object_id
    helper->register_servant(orb, sender1_poa, sender1_impl, "Sender1");
    helper->register_servant(orb, sender2_poa, sender2_impl, "Sender2");
    
    
    sender1_poa->the_POAManager()->activate();
    sender2_poa->the_POAManager()->activate();

    //Use execution class to wait for interrupt
    exe->Start();

    orb->shutdown(true);
    
    return 0;
}