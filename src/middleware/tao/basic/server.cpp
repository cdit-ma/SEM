#include <iostream>
#include <thread>
#include "server.h"
#include "tao/IORTable/IORTable.h"
#include <middleware/tao/helper.h>
#include <future>

Hello::Hello (CORBA::ORB_ptr orb, std::string id)
: orb_ (CORBA::ORB::_duplicate (orb))
{
    this->id = id;
}


void Hello::send(const Test::Message& message){
    std::cout << "\tID: " << id << std::endl;
    std::cout << "\ttime: " << message.time << std::endl;
    std::cout << "\tinst_name: " << message.inst_name << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "\tinst_name: SLEPT" << std::endl;
}

//THIS
//bin/tao_server -ORBEndpoint iiop://192.168.111.90:50001

//OTHER
//bin/tao_client -ORBInitRef Sender2=corbaloc:iiop:192.168.111.90:50001/Sender1


//TODO, look at how we can bind without passing parameters to the ORB!
//Clean Client and server
//Test multithreadedly
int main(int argc, char ** argv){
    auto helper = tao::TaoHelper::get_tao_helper();
    
    auto orb = helper->get_orb("iiop://192.168.111.90:50001");

    auto future = std::async(std::launch::async, [=](){
         //Run the ORB
        orb->run();
        orb->destroy();
    });
    
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
    
    
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Activating Sender1" << std::endl;
    sender1_poa->the_POAManager()->activate();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    sender1_poa->the_POAManager()->deactivate(true, true);

    //     sender2_poa->the_POAManager()->activate();
    //     std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    //     sender2_poa->the_POAManager()->deactivate(true, true);

    std::cout << "SHUTTING DOWN" << std::endl;
    orb->shutdown(true);
    std::cout << "Shutdown" << std::endl;
    

    return 0;
}