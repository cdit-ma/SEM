#ifndef NODEMANAGERPROTOHANDLER_H
#define NODEMANAGERPROTOHANDLER_H

#include "aggregationprotohandler.h"

#include <proto/controlmessage/controlmessage.pb.h>

#include <utility>

class NodeManagerProtoHandler : public AggregationProtoHandler {
public:
    NodeManagerProtoHandler(std::shared_ptr<DatabaseClient> db_client,
                            ExperimentTracker& exp_tracker) :
        AggregationProtoHandler(std::move(db_client), exp_tracker){};

    void BindCallbacks(zmq::ProtoReceiver& ProtoReceiver) final;

private:
    struct ExpStateCreationInfo {
        int experiment_id;
        int experiment_run_id;
        // REVIEW(Jackson): Make a proper named type for these pairs
        std::vector<std::pair<int, std::string>> pubsub_connections;
        std::vector<std::pair<int, std::string>> reqrep_connections;
    };

    // Process environment manager callbacks
    void ProcessEnvironmentMessage(const NodeManager::EnvironmentMessage& message);
    void ProcessGetInfoControlMessage(const NodeManager::ControlMessage& message);
    void ProcessConfigureControlMessage(const NodeManager::ControlMessage& message);
    void ProcessShutdownControlMessage(const NodeManager::ControlMessage& message);
    void ProcessNode(const NodeManager::Node& message, ExpStateCreationInfo& exp_info);
    void ProcessContainer(const NodeManager::Container& message,
                          ExpStateCreationInfo& exp_info,
                          int node_id);
    void ProcessComponent(const NodeManager::Component& message,
                          ExpStateCreationInfo& exp_info,
                          int container_id);
    void ProcessPort(const NodeManager::Port& message,
                     ExpStateCreationInfo& exp_info,
                     int component_instance_id,
                     const std::string& component_instance_location);
    void ProcessWorker(const NodeManager::Worker& message,
                       ExpStateCreationInfo& exp_info,
                       int component_instance_id,
                       const std::string& worker_path);
    void ProcessLogger(const NodeManager::Logger& message, ExpStateCreationInfo& exp_info);

    void ProcessPortConnections(const ExpStateCreationInfo& exp_info);
    void ProcessPubSubConnection(int from_port_id,
                                 int experiment_run_id,
                                 const std::string& to_port_graphml);
    void ProcessReqRepConnection(int from_port_id,
                                 int experiment_run_id,
                                 const std::string& to_port_graphml);
};

#endif