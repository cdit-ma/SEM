// Include the proto convert functions for the port type
#include "../../base/basic.h"
#include "../../proto/basic.pb.h"

#include <future>
#include <core/component.h>
#include <middleware/zmq/requestreply/requesterport.hpp>
#include <middleware/zmq/requestreply/replierport.hpp>
#include <middleware/zmq/pubsub/subscriberport.hpp>
#include <core/ports/pubsub/subscriberport.hpp>
#include <core/ports/libportexport.h>

Base::Basic callback(Base::Basic& message){
    std::cout << "RUNNING: callback" << std::endl;
    std::cout << "Int: " << message.int_val << std::endl;
    std::cout << "str_val: " << message.str_val << std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(message.int_val * 10));
    
    //Base::Basic m2;
    message.int_val *= 10;
    message.str_val = "10asdasd";
    return message;
}

void callback2(Base::Basic& message){
    std::cout << "RUNNING: callback2" << std::endl;
    //std::cout << "Int: " << message.int_val << std::endl;
    //std::cout << "str_val: " << message.str_val << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(message.int_val * 10));
    
    //Base::Basic m2;
    message.int_val *= 10;
}

Base::Basic callback3(){
    Base::Basic m2;
    m2.int_val *= 10;
    return m2;
}


void print_base(const Base::Basic& m){
    std::cerr << "Base::Basic(" << m.get_base_message_id() << ")" << std::endl;
    std::cerr << "\tint_val: " << m.int_val << std::endl;
    std::cerr << "\tstr_val: " << m.str_val << std::endl;
    if(m.int_val == 5){
        //std::cerr << "\tsleepybois" << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void print_base2(){
    std::cerr << "" << std::endl;
}

void configure_port(Port& port, const std::string& port_address){
    auto address = port.GetAttribute("server_address").lock();
    address->set_String(port_address);
}

int main(int ac, char* av[])
{
    auto component = std::make_shared<Component>("Test");
    component->AddCallback<Base::Basic, Base::Basic>("asdasd2", new CallbackWrapper<Base::Basic, Base::Basic>(callback));
    component->AddCallback<void, Base::Basic>("asdasd", new CallbackWrapper<void, Base::Basic>(callback2));
    component->AddCallback<Base::Basic, void>("asdasd3", new CallbackWrapper<Base::Basic, void>(callback3));
    //component->AddCallback<Base::Basic>("asdasd", print_base);
    
    auto requester_port = ConstructRequesterPort<zmq::RequesterPort<void, void, Base::Basic, ::Basic>>("Test23", component);
    auto replier_port = ConstructReplierPort<zmq::ReplierPort<void, void, Base::Basic, ::Basic>>("asdasd", component);

    auto requester_port2 = ConstructRequesterPort<zmq::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic>>("Test24", component);
    auto replier_port2 = ConstructReplierPort<zmq::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic>>("asdasd2", component);
    


    auto address = "inproc://testy";
    auto address2 = "inproc://testy2";

    configure_port(*requester_port, address);
    configure_port(*replier_port, address);

    configure_port(*requester_port2, address2);
    configure_port(*replier_port2, address2);

    requester_port->Configure();
    replier_port->Configure();

    requester_port2->Configure();
    replier_port2->Configure();

    requester_port->Activate();
    replier_port->Activate();

    requester_port2->Activate();
    replier_port2->Activate();


    for(int i = 0; i < 1; i++){
        Base::Basic a;
        a.int_val = i;
        a.str_val = "LEL" + std::to_string(i);
        std::cerr << "Sending A" << std::endl;
        print_base(a);

        
        auto response = requester_port->SendRequest(a, std::chrono::milliseconds(100));
        auto response2 = requester_port2->SendRequest(a, std::chrono::milliseconds(100));

        if(response){
            std::cerr << "Got Response1" << std::endl;
        }
        

        if(response2.first){
            std::cerr << "Got Response 2: " << std::endl;
            print_base(response2.second);
        }
        std::cerr << std::endl;
    }

    requester_port->Passivate();
    replier_port->Passivate();

    requester_port2->Passivate();
    replier_port2->Passivate();
    
    requester_port->Terminate();
    replier_port->Terminate();

    requester_port2->Terminate();
    replier_port2->Terminate();



    delete requester_port;
    delete replier_port;
    delete requester_port2;
    delete replier_port2;


    return 0;
}