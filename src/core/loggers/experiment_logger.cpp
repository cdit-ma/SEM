#include "experiment_logger.h"
#include <proto/modelevent/modelevent.pb.h>
#include <zmq/protowriter/cachedprotowriter.h>
#include <google/protobuf/util/time_util.h>

void FillInfoPB(ModelEvent::Info& info, Logan::ExperimentLogger& logger){
    info.set_experiment_name(logger.GetExperimentName());
    using namespace google::protobuf::util;
    auto timestamp = TimeUtil::MicrosecondsToTimestamp(Logger::GetCurrentTime().count());
    info.mutable_timestamp()->Swap(&timestamp);
}


Logan::ExperimentLogger::ExperimentLogger(const std::string& experiment_name, const std::string& endpoint, ::Logger::Mode mode):
    experiment_name_(experiment_name)
{
    switch(mode){
        case Logger::Mode::LIVE:{
            writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::ProtoWriter());
            break;
        }
        case Logger::Mode::CACHED:{
            writer_ = std::unique_ptr<zmq::ProtoWriter>(new zmq::CachedProtoWriter());
            break;
        }
        default:{
            throw std::runtime_error("Invalid logging Mode");
        }
    }
    if(!writer_ || !writer_->BindPublisherSocket(endpoint)){
        throw std::runtime_error("Cannot bind endpoint: " + endpoint);
    }
}


void Logan::ExperimentLogger::LogLifecycleEvent(const ::Logger::LifeCycleEvent& event){
    auto event_pb = std::unique_ptr<ModelEvent::LifecycleEvent>(new ModelEvent::LifecycleEvent());
    event_pb->set_type((ModelEvent::LifecycleEvent::Type)(int)event);
    FillInfoPB(*(event_pb->mutable_info()), *this);
    PushMessage(std::move(event_pb)); 
}

void Logan::ExperimentLogger::PushMessage(std::unique_ptr<google::protobuf::MessageLite> message){
    if(writer_){
        writer_->PushMessage("ExperimentEvent*", std::move(message));
    }
}


const std::string& Logan::ExperimentLogger::GetExperimentName() const{
    std::lock_guard<std::mutex> lock(mutex_);
    return experiment_name_;
}