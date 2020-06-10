#include "modeleventprotohandler.h"

#include "utils.h"

#include "databaseclient.h"
#include "experimenttracker.h"

#include <proto/modelevent/modelevent.pb.h>
#include <zmq/protoreceiver/protoreceiver.h>

#include <google/protobuf/util/time_util.h>

#include <chrono>

#include <functional>

using google::protobuf::util::TimeUtil;

void ModelEventProtoHandler::BindCallbacks(zmq::ProtoReceiver& receiver)
{
    receiver.RegisterProtoCallback<ModelEvent::LifecycleEvent>(
        std::bind(&ModelEventProtoHandler::ProcessLifecycleEvent, this, std::placeholders::_1));
    receiver.RegisterProtoCallback<ModelEvent::WorkloadEvent>(
        std::bind(&ModelEventProtoHandler::ProcessWorkloadEvent, this, std::placeholders::_1));
    receiver.RegisterProtoCallback<ModelEvent::UtilizationEvent>(
        std::bind(&ModelEventProtoHandler::ProcessUtilizationEvent, this, std::placeholders::_1));
}

void ModelEventProtoHandler::ProcessLifecycleEvent(const ModelEvent::LifecycleEvent& message)
{
    // REVIEW(Jackson): Rework this function once we've reworked lifecycle event protobuf protocol
    // REVIEW(Jackson): Add worker lifecycle support
    if(message.has_component()) {
        if(message.has_port()) {
            try {
                InsertPortLifecycleEvent(message.info(), message.type(), message.component(),
                                         message.port());
            } catch(const std::exception& e) {
                std::cerr << "An exception occured while trying to insert a PortLifecycleEvent: "
                          << e.what() << std::endl;
            }
        } else {
            InsertComponentLifecycleEvent(message.info(), message.type(), message.component());
        }
    }
}

void ModelEventProtoHandler::ProcessWorkloadEvent(const ModelEvent::WorkloadEvent& message)
{
    std::string worker_instance_id = database_->quote(
        GetWorkerInstanceID(message.component(), message.worker()));
    std::string function = database_->quote(message.function_name());
     std::string workload_id = database_->quote(message.workload_id());
    std::string type = database_->quote(ModelEvent::WorkloadEvent::Type_Name(message.event_type()));
    std::string args = database_->quote(message.args());
    std::string log_level = database_->quote(message.log_level());
    // REVIEW(Jackson): What is the status with the requirement to not quote timestamps for
    //  formatting reasons
    std::string sample_time = database_->quote(TimeUtil::ToString(message.info().timestamp()));

    database_->InsertValues("WorkloadEvent",
                            {"WorkerInstanceID", "WorkloadID", "Function", "Type", "Arguments",
                             "LogLevel", "SampleTime"},
                            {worker_instance_id, workload_id, function, type, args, log_level,
                             sample_time});
    database_->UpdateLastSampleTime(experiment_run_id_, sample_time);
}

void ModelEventProtoHandler::ProcessUtilizationEvent(const ModelEvent::UtilizationEvent& message)
{
    std::string port_id = database_->quote(GetPortID(message.port(), message.component()));

    auto port_id_acquired_time = std::chrono::steady_clock::now();

    std::string seq_num = database_->quote(message.port_event_id());
    std::string type = database_->quote(ModelEvent::UtilizationEvent::Type_Name(message.type()));
    std::string msg = database_->quote(message.message());
    std::string sample_time = database_->quote(TimeUtil::ToString(message.info().timestamp()));

    database_->InsertValues("PortEvent",
                            {"PortID", "PortEventSequenceNum", "Type", "Message", "SampleTime"},
                            {port_id, seq_num, type, msg, sample_time});
    database_->UpdateLastSampleTime(experiment_run_id_, sample_time);
}

void ModelEventProtoHandler::InsertComponentLifecycleEvent(
    const ModelEvent::Info& info,
    const ModelEvent::LifecycleEvent_Type& type,
    const ModelEvent::Component& component)
{
    std::string component_instance_id = database_->quote(GetComponentInstanceID(component));
    std::string sample_time = database_->quote(TimeUtil::ToString(info.timestamp()));
    std::string type_string = database_->quote(ModelEvent::LifecycleEvent::Type_Name(type));

    database_->InsertValues("ComponentLifecycleEvent",
                            {"ComponentInstanceID", "SampleTime", "Type"},
                            {component_instance_id, sample_time, type_string});
    database_->UpdateLastSampleTime(experiment_run_id_, sample_time);
}

void ModelEventProtoHandler::InsertPortLifecycleEvent(const ModelEvent::Info& info,
                                                      const ModelEvent::LifecycleEvent_Type& type,
                                                      const ModelEvent::Component& component,
                                                      const ModelEvent::Port& port)
{
    std::string port_id = database_->quote(GetPortID(port, component));
    std::string sample_time = database_->quote(TimeUtil::ToString(info.timestamp()));
    std::string type_string = database_->quote(ModelEvent::LifecycleEvent::Type_Name(type));

    database_->InsertValues("PortLifecycleEvent", {"PortID", "SampleTime", "Type"},
                            {port_id, sample_time, type_string});
    database_->UpdateLastSampleTime(experiment_run_id_, sample_time);
}

int ModelEventProtoHandler::GetComponentInstanceID(const ModelEvent::Component& component_instance)
{
    try {
        return component_inst_id_cache_.at(component_instance.id());
    } catch(const std::out_of_range& oor_ex) {
        int component_id = GetComponentID(component_instance.type());

        std::string&& comp_id = database_->quote(component_id);
        std::string&& graphml_id = database_->quote(component_instance.id()); // Graphml ID,
                                                                              // not database
                                                                              // row ID

        std::stringstream condition_stream;
        condition_stream << "ComponentID = " << comp_id << " AND GraphmlID = " << graphml_id;

        int component_inst_id = database_->GetID("ComponentInstance", condition_stream.str());
        component_inst_id_cache_.emplace(
            std::make_pair(component_instance.id(), component_inst_id));
        return component_inst_id;
    }
}

int ModelEventProtoHandler::GetPortID(const ModelEvent::Port& port,
                                      const ModelEvent::Component& component)
{
    try {
        return port_id_cache_.at(port.id());
    } catch(const std::out_of_range& oor_ex) {
        int component_instance_id = GetComponentInstanceID(component);

        std::string&& comp_inst_id = database_->quote(component_instance_id);
        std::string&& name = database_->quote(port.name());

        std::stringstream condition_stream;
        condition_stream << "ComponentInstanceID = " << comp_inst_id << " AND Name = " << name;

        int port_id = database_->GetID("Port", condition_stream.str());
        port_id_cache_.emplace(std::make_pair(port.id(), port_id));
        return port_id;
    }
}

int ModelEventProtoHandler::GetWorkerInstanceID(const ModelEvent::Component& component,
                                                const ModelEvent::Worker& worker_instance)
{
    try {
        return worker_inst_id_cache_.at(worker_instance.id());
    } catch(const std::out_of_range& oor_ex) {
        int component_instance_id = GetComponentInstanceID(component);

        std::string&& comp_inst_id = database_->quote(component_instance_id);
        std::string&& name = database_->quote(worker_instance.name());

        std::stringstream condition_stream;
        condition_stream << "ComponentInstanceID = " << comp_inst_id << " AND Name = " << name;

        int worker_inst_id = database_->GetID("WorkerInstance", condition_stream.str());
        worker_inst_id_cache_.emplace(std::make_pair(worker_instance.id(), worker_inst_id));
        return worker_inst_id;
    }
}

int ModelEventProtoHandler::GetComponentID(const std::string& name)
{
    try {
        return component_id_cache_.at(name);
    } catch(const std::out_of_range& oor_ex) {
        std::stringstream condition_stream;
        condition_stream << "Name = " << database_->quote(name)
                         << " AND ExperimentRunID = " << experiment_run_id_;

        int component_id = database_->GetID("Component", condition_stream.str());
        component_id_cache_.emplace(std::make_pair(name, component_id));
        return component_id;
    }
}
