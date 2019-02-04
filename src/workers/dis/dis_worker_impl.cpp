#include "dis_worker_impl.h"
#include <iostream>

Dis_Worker_Impl::Dis_Worker_Impl(){

}

void Dis_Worker_Impl::Connect(const std::string& ip_address, const int port){
}

void Dis_Worker_Impl::Disconnect(){
}



void Dis_Worker_Impl::ProcessEvents(const std::string& ip_address, const int port){
    
    KDIS::NETWORK::Connection connection(ip_address, port);

    while(true){
        try{
            // Note: GetNextPDU supports PDU Bundling, which Receive does not.
            connection.GetNextPDU();
        }catch(const std::exception & e){
            std::cout << e.what() << std::endl;
        }
    }
}
