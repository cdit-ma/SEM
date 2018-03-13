#include <iostream>
#include <future>
#include <unordered_map>
#include <set>

#include <middleware/tao/helper.h>

#include "message2S.h"


int main(int argc, char** argv){

    auto helper = tao::TaoHelper::get_tao_helper();
    auto orb = helper->get_orb("iiop://192.168.111.90:50007");
    
    //Run the orb
    auto orb_future = std::async(std::launch::async, [orb](){
        if(orb){
            std::cout << "Starting the Orb" << std::endl;
            orb->run();
            std::cout << "Stopping the Orb" << std::endl;
            orb->destroy();
        }
    });

    std::string server_addr("corbaloc:iiop:192.168.111.90:50007");

    std::string sender_1_name("Sender1");
    std::string sender_2_name("Sender2");

    std::string sender_1_addr(server_addr + "/" + sender_1_name);
    std::string sender_2_addr(server_addr + "/" + sender_2_name);

    helper->register_initial_reference(orb, sender_1_name, sender_1_addr);
    helper->register_initial_reference(orb, sender_2_name, sender_2_addr);

    std::set<std::string> unregistered_references;
    std::set<std::string> registered_references;

    unregistered_references.insert(sender_1_name);
    unregistered_references.insert(sender_2_name);

    std::unordered_map<std::string, CORBA::Object_ptr> object_ptrs;
    std::unordered_map<std::string, Test::Hello_ptr> senders;


    Test::Message2 message;
    message.inst_name2 = "=D";
    message.time2 = argc;
    

    while(true){
        for (auto itt = unregistered_references.begin(); itt != unregistered_references.end();) {
            auto reference_str = *itt;
            try{
                //std::cout << "Registering: " << reference_str << std::endl;
                auto ptr = helper->resolve_initial_references(orb, reference_str);
                if(ptr){
                    object_ptrs[reference_str] = ptr;
                    senders[reference_str] = Test::Hello::_narrow(ptr);
                    registered_references.insert(reference_str);
	                itt = unregistered_references.erase(itt);
                    std::cout << "Registered: " << reference_str << std::endl;
                }
            }catch(...){
                ++itt;
            }
        }


        for (auto itt = registered_references.begin(); itt != registered_references.end();) {
            auto reference_str = *itt;
            try{
                std::cout << "SENDING A HECK" << std::endl;
                message.time2++;
                //std::cout << "Sending: " << registered_reference << std::endl;
                auto sender = senders[reference_str];
                if(sender){
                    sender->send22(message);
                }
                ++itt;

            }catch(...){
                std::cout << "Ref Ded: " << reference_str << std::endl;
                //delete object_ptrs[reference_str];
                itt = registered_references.erase(itt);
                unregistered_references.insert(reference_str);
            }
        }
    }

    orb->shutdown(true);

    return 0;
}