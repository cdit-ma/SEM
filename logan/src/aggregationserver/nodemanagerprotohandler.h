#ifndef NODEMANAGERPROTOHANDLER_H
#define NODEMANAGERPROTOHANDLER_H

#include "aggregationprotohandler.h"

#include <proto/controlmessage/controlmessage.pb.h>

class NodeManagerProtoHandler : public AggregationProtoHandler {
public:
    NodeManagerProtoHandler(std::shared_ptr<DatabaseClient> db_client,
                            ExperimentTracker& exp_tracker) :
        AggregationProtoHandler(db_client, exp_tracker){};

    void BindCallbacks(zmq::ProtoReceiver& ProtoReceiver);

private:
    // Process environment manager callbacks
    void ProcessEnvironmentMessage(const NodeManager::EnvironmentMessage& message);
    void ProcessGetInfoControlMessage(const NodeManager::ControlMessage& message);
    void ProcessConfigureControlMessage(const NodeManager::ControlMessage& message);
    void ProcessShutdownControlMessage(const NodeManager::ControlMessage& message);
    void ProcessNode(const NodeManager::Node& message, int experiment_run_id);
    void
    ProcessContainer(const NodeManager::Container& message, int experiment_run_id, int node_id);
    void ProcessComponent(const NodeManager::Component& message,
                          int experiment_run_id,
                          int container_id);
    void ProcessPort(const NodeManager::Port& message,
                     int experiment_run_id,
                     int component_instance_id,
                     const std::string& component_instance_location);
    void ProcessPortConnection(int from_port_id,
                               int experiment_run_id,
                               const std::string& to_port_graphml);
    void ProcessWorker(const NodeManager::Worker& message,
                       int experiment_run_id,
                       int component_instance_id,
                       const std::string& worker_path);
    void ProcessLogger(const NodeManager::Logger& message, int experiment_run_id);
};

#endif