// Include the proto convert functions for the port type
#include "../base/basic.h"
#include "../proto/basic.pb.h"

#include <future>

#include <middleware/zmq/requestreply/requesterport.hpp>
#include <middleware/zmq/requestreply/replierport.hpp>

Base::Basic callback(Base::Basic& message){
    std::cout << "GOT MESSAGE: " << std::endl;
    std::cout << "Int: " << message.int_val << std::endl;
    std::cout << "str_val: " << message.str_val << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(message.int_val * 10));
    message.int_val *= 10;
    return message;
}

int main(int ac, char* av[])
{
    auto client_port = new zmq::RequesterPort<Base::Basic, ::Basic, Base::Basic, ::Basic>(std::weak_ptr<Component>(), "ClientEventPort");
    auto server_port = new zmq::ReplierPort<Base::Basic, ::Basic, Base::Basic, ::Basic>(std::weak_ptr<Component>(), "ServerEventPort", callback);

    auto address = "inproc://testy";
    auto address2 = "inproc://testy2";

    auto out_address = client_port->GetAttribute("server_address").lock();
    auto in_address = server_port->GetAttribute("server_address").lock();

    std::cout << " GOT ADDRESSES" << std::endl;

    out_address->set_String(address);
    std::cout << " GOT ADDRESSES" << std::endl;
    in_address->set_String(address2);
    std::cout << " GOT ADDRESSES" << std::endl;

    std::cout << "CONFIGURE" << std::endl;
    
    server_port->Configure();
    client_port->Configure();

    std::cout << "ACTIVATE" << std::endl;
    server_port->Activate();
    client_port->Activate();

    std::cout << "STARTING" << std::endl;

    auto test = std::async(std::launch::async, [=](){
        int send_count = 20;
        //Send as fast as possible
        for(int i = 0; i < send_count; i++){
            Base::Basic b;
            b.int_val = i;
            b.str_val = std::to_string(i);
            std::cout << "SENDING: " << i << std::endl;
            auto c = client_port->SendRequest(b, std::chrono::milliseconds(100));
            if(c.first){
                std::cout << "RX: " << c.second.int_val << std::endl;
                std::cout << "RX: " << c.second.str_val << std::endl;
            }
        }
    });

    
    test.wait();

    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "KILLING: " << std::endl;

    
    std::cout << "Passivate: " << std::endl;
    server_port->Passivate();
    client_port->Passivate();
    std::cout << "Passivated: " << std::endl;
    
    std::cout << "Terminate: " << std::endl;
    server_port->Terminate();
    std::cout << "Terminate2: " << std::endl;
    client_port->Terminate();
    std::cout << "Terminated: " << std::endl;



    delete client_port;
    delete server_port;

    return 0;
}