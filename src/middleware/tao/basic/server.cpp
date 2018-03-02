#include <iostream>
#include <future>
#include <thread>
#include <signal.h>

#include <middleware/tao/helper.h>
#include <nodemanager/execution.hpp>

#include "server.h"

Hello::Hello (CORBA::ORB_ptr orb, std::string id)
: orb_ (CORBA::ORB::_duplicate (orb))
{
    this->id = id;
}

Execution* exe = 0;

void signal_handler(int sig)
{
    exe->Interrupt();
}


void Hello::sendLelbs(const Test::Message& message){
    std::cout << "\tID: " << id << std::endl;
    std::cout << "\ttime: " << message.time << std::endl;
    std::cout << "\tinst_name: " << message.inst_name << std::endl;
    std::cout << "\tBlocking" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "\tDone" << std::endl;
}

int main(int argc, char ** argv){
    signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
    exe = new Execution();
    auto helper = tao::TaoHelper::get_tao_helper();
    auto orb = helper->get_orb("iiop://192.168.111.90:50005");

    if(!orb){
        std::cerr << "DED RAT" << std::endl;
        return -1;
    }
    
    
    // Create the child POA for the test logger factory servants.
    auto sender1_poa = helper->get_poa(orb, "Sender1");
    auto sender2_poa = helper->get_poa(orb, "Sender2");
    
    //Construct a sender
    auto sender1_impl = new Hello(orb, "Sender1");
    auto sender2_impl = new Hello(orb, "Sender2");

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