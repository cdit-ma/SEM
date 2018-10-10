#include "loganlogger.h"
#include <proto/modelevent/modelevent.pb.h>



LoganLogger::LoganLogger(const std::string& experiment_name, const std::string& host_name, const std::string& address, const std::string& port, Logger::Mode mode):
    experiment_name_(experiment_name),
    host_name_(host_name)
{
    const auto& endpoint = "tcp://" + address + ":" + port;
        
    switch(mode){
        case Mode::LIVE:{
            writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter());
            break;
        }
        case Mode::CACHED:{
            writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::CachedProtoWriter());
            break;
        }
        default:{
            throw std::runtime_error("Invalid logging Mode");
        }
    }
    if(!writer_->BindPublisherSocket(endpoint)){
        throw std::runtime_error("Cannot bind endpoint: " + endpoint);
    }
}

void LoganLogger::LogLifecycleException(const Activatable& entity, const std::string& message){
std::cerr << GET_FUNC << std::endl;
}

void LoganLogger::LogWorkerEvent(const Worker& worker, const std::string& function_name, const Logger::WorkloadEvent& event, int work_id, std::string args, int message_log_level){
std::cerr << GET_FUNC << std::endl;
}

void LoganLogger::LogComponentMessage(const Component& component, const std::string& message){
std::cerr << GET_FUNC << std::endl;
}

void LoganLogger::LogLifecycleEvent(const Component& component, const Logger::LifeCycleEvent& event){
std::cerr << GET_FUNC << std::endl;
}

void LoganLogger::LogLifecycleEvent(const Port& port, const Logger::LifeCycleEvent& event){
    std::cerr << GET_FUNC << std::endl;
}

void LoganLogger::LogComponentEvent(const Port& port, const ::BaseMessage& message, const Logger::ComponentEvent& event){
std::cerr << GET_FUNC << std::endl;
}

void LoganLogger::LogPortExceptionEvent(const Port& port, const ::BaseMessage& message, const std::string& error_string){
std::cerr << GET_FUNC << std::endl;
}

void LoganLogger::LogPortExceptionEvent(const Port& port, const std::string& error_string){
std::cerr << GET_FUNC << std::endl;
}