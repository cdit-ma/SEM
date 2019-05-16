#include <memory>

#include "tcp_worker.h"
#include "tcpworkerimpl.h"
#include <iostream>
#include <utility>

const std::string& Tcp_Worker::get_version() const
{
    const static std::string worker_version{"0.1.0"};
    return worker_version;
}

Tcp_Worker::Tcp_Worker(const BehaviourContainer& container, const std::string& inst_name) :
    Worker(container, GET_FUNC, inst_name)
{
    ip_address_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "ip_address").lock();
    port_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::STRING, "port").lock();
    delimiter_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::CHARACTER, "delimiter").lock();
    impl_ = std::make_shared<TcpWorkerImpl>();
}

Tcp_Worker::~Tcp_Worker() { impl_.reset(); }

bool Tcp_Worker::SetTcpCallback(const TcpWorkerImpl::CallbackFunctionType& callback_function)
{
    return impl_->BindCallback(callback_function);
}

bool Tcp_Worker::Connect()
{
    auto work_id = get_new_work_id();
    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id, "Connecting TCP worker");

    try {
        impl_->Connect(ip_address_->get_String(), port_->get_String());
    } catch (const std::exception& ex) {
        LogException(GET_FUNC, ex, work_id);
        return false;
    }

    Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, work_id);
    return true;
}

void Tcp_Worker::Disconnect()
{
    // Only valid if we're a receiver
    auto work_id = get_new_work_id();
    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id, "Disconnecting TCP worker");
    try {
        impl_->Disconnect();
    } catch(const std::exception& ex) {
        LogException(GET_FUNC, ex, work_id);
    }
    Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, work_id);
}

bool Tcp_Worker::Bind()
{
    auto work_id = get_new_work_id();
    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id, "Binding TCP worker address");
    try {
        impl_->Bind(ip_address_->get_String(), port_->get_String());
    } catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
        LogException(GET_FUNC, ex, work_id);
        return false;
    }
    // Bound successfully
    Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, work_id);
    return true;
}

int Tcp_Worker::Send(std::vector<char> data)
{
    auto work_id = get_new_work_id();
    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id, "TCP worker sending data");
    try {
        return impl_->Send(std::move(data));
    } catch(const std::exception& ex) {
        LogException(GET_FUNC, ex, work_id);
        return 0;
    }
}

int Tcp_Worker::Receive(std::vector<char>& out)
{
    auto work_id = get_new_work_id();
    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id, "TCP worker receiving data");
    try {
        return impl_->Receive(out);
    } catch(const std::exception& ex) {
        LogException(GET_FUNC, ex, work_id);
        return 1;
    }
}

void Tcp_Worker::HandleTerminate() {
    Worker::HandleTerminate();
}
void Tcp_Worker::HandleConfigure(){
    if(delimiter_) {
       impl_->SetDelimiter(delimiter_->get_Character());
    }
    Worker::HandleTerminate();
}
