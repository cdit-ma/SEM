#include "dis_worker.h"
#include "dis_worker_impl.h"

Dis_Worker::Dis_Worker(const BehaviourContainer& container, const std::string& inst_name) : Worker(container, GET_FUNC, inst_name){
    impl_ = std::unique_ptr<Dis_Worker_Impl>(new Dis_Worker_Impl());

    ip_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "ip_address").lock();
    port_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "port").lock();
    if(port_){
        port_->set_Integer(3000);
    }
}

const std::string& Dis_Worker::get_version() const{
    const static std::string WORKER_VERSION{"0.1.0"};
    return WORKER_VERSION;
}

Dis_Worker::~Dis_Worker(){
    impl_.reset();
}

void Dis_Worker::SetPduCallback(std::function<void (const KDIS::PDU::Header &)> func){
    impl_->SetPduCallback(func);
}

std::string Dis_Worker::PDU2String(const KDIS::PDU::Header& header){
    return impl_->PDU2String(header);
}

bool Dis_Worker::Connect(){
    try{
        Connect_();
        return true;
    }catch(const std::exception& e){

    }
    return false;
}

void Dis_Worker::Disconnect(){
    Disconnect_();
}
        
void Dis_Worker::Connect_(){
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
    Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, work_id);
}

void Dis_Worker::Disconnect_(){
    auto work_id = get_new_work_id();
    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id, "Disconnecting DIS Worker");
    impl_->Disconnect();
    Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, work_id);
}

void Dis_Worker::HandleConfigure() {
    Connect_();
    Worker::HandleConfigure();
}

void Dis_Worker::HandleTerminate() {
    Disconnect_();
    Worker::HandleTerminate();
}