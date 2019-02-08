#include "dis_worker.h"
#include "dis_worker_impl.h"

Dis_Worker::Dis_Worker(const BehaviourContainer& container, const std::string& inst_name) : Worker(container, GET_FUNC, inst_name){
    impl_ = std::unique_ptr<Dis_Worker_Impl>(new Dis_Worker_Impl());

    ip_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "ip_address").lock();
    port_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "port").lock();
    if (port_ != NULL) {
        port_->set_Integer(3000);
    }
}


Dis_Worker::~Dis_Worker(){
    impl_.reset();
}

void Dis_Worker::SetPduCallback(std::function<void (const KDIS::PDU::Header &)> func){
    impl_->SetPduCallback(func);
}


void Dis_Worker::HandleConfigure() {
    auto work_id = get_new_work_id();

    std::string ip_address;
    int port;
    
    if(ip_address_) {
        ip_address = ip_address_->get_String();
    }

    if(port_) {
        port = port_->get_Integer();
    }


    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id,
            "Connecting DIS Worker: " + ip_address + ":" + std::to_string(port));
    try{
        impl_->Connect(ip_address, port);
    }catch(const std::exception& ex){
        LogException(GET_FUNC, ex, work_id);
        throw;
    }
    Worker::HandleConfigure();
}

void Dis_Worker::HandleTerminate() {
    auto work_id = get_new_work_id();
    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id, "Disconnecting DIS Worker");

    impl_->Disconnect();
    Worker::HandleTerminate();
}