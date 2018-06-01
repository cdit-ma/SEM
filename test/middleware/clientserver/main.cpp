// Include the proto convert functions for the port type
#include "../base/basic.h"
#include "../proto/basic.pb.h"

#include <future>

#include <middleware/zmq/clientserver/clientport.hpp>
#include <middleware/zmq/clientserver/serverport.hpp>

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
    auto client_port = new zmq::ClientEventPort<Base::Basic, ::Basic, Base::Basic, ::Basic>(std::weak_ptr<Component>(), "TEST");
    auto server_port = new zmq::ServerEventPort<Base::Basic, ::Basic, Base::Basic, ::Basic>(std::weak_ptr<Component>(), "TEST", callback);

    auto address = "inproc://testy";

    auto out_address = client_port->GetAttribute("server_address").lock();
    auto in_address = server_port->GetAttribute("server_address").lock();

    std::cout << " GOT ADDRESSES" << std::endl;

    out_address->StringList().push_back(address);
    in_address->StringList().push_back(address);

    std::cout << "CONFIGURE" << std::endl;
    
    server_port->Configure();
    client_port->Configure();

    std::cout << "ACTIVATE" << std::endl;
    server_port->Activate();
    client_port->Activate();

    std::cout << "STARTING" << std::endl;

    auto test = std::async(std::launch::async, [=](){
        int send_count = 50;
        //Send as fast as possible
        for(int i = 0; i < send_count; i++){
            Base::Basic b;
            b.int_val = i;
            b.str_val = std::to_string(i);
            std::cout << "SENDING: " << i << std::endl;
            auto c = client_port->tx(b);
            
            std::cout << "RX: " << c.int_val << std::endl;
            std::cout << "RX: " << c.str_val << std::endl;
        }
    });

    
    test.wait();

    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "KILLING: " << std::endl;

    

    server_port->Passivate();
    client_port->Passivate();
    server_port->Terminate();
    client_port->Terminate();



    delete client_port;
    delete server_port;

    return 0;
}