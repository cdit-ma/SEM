#include "dis_worker_impl.h"

Dis_Worker_Impl::Dis_Worker_Impl(){

}

void Dis_Worker_Impl::Connect(const std::string& ip_address, const int port){
    using namespace KDIS::NETWORK;
    connection_ = std::unique_ptr<Connection>(new Connection(ip_address.c_str(), port));
}

void Dis_Worker_Impl::Disconnect(){

}