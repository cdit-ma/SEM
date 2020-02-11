#ifndef AGGREGATIONREPLIER_H
#define AGGREGATIONREPLIER_H

#include "../databaseclient.h"
#include <zmq/protoreplier/protoreplier.hpp>

#include <proto/aggregationmessage/aggregationmessage.pb.h>

namespace re::logging::aggregation::broker {

class AggregationReplier : public zmq::ProtoReplier {
public:
    explicit AggregationReplier(std::shared_ptr<DatabaseClient> db_client);

    std::unique_ptr<AggServer::ExperimentRunResponse>
    ProcessExperimentRunRequest(const AggServer::ExperimentRunRequest& message);

    std::unique_ptr<AggServer::ExperimentStateResponse>
    ProcessExperimentStateRequest(const AggServer::ExperimentStateRequest& message);

    std::unique_ptr<AggServer::PortLifecycleResponse>
    ProcessPortLifecycleRequest(const AggServer::PortLifecycleRequest& message);

    std::unique_ptr<AggServer::PortEventResponse>
    ProcessPortEventRequest(const AggServer::PortEventRequest& message);

    std::unique_ptr<AggServer::WorkloadResponse>
    ProcessWorkloadEventRequest(const AggServer::WorkloadRequest& message);

    std::unique_ptr<AggServer::MarkerResponse>
    ProcessMarkerRequest(const AggServer::MarkerRequest& message);

    std::unique_ptr<AggServer::CPUUtilisationResponse>
    ProcessCPUUtilisationRequest(const AggServer::CPUUtilisationRequest& message);

    std::unique_ptr<AggServer::MemoryUtilisationResponse>
    ProcessMemoryUtilisationRequest(const AggServer::MemoryUtilisationRequest& message);

    std::unique_ptr<AggServer::NetworkUtilisationResponse>
    ProcessNetworkUtilisationRequest(const AggServer::NetworkUtilisationRequest& message);

private:
    std::shared_ptr<DatabaseClient> database_;

    void FillNodeState(AggServer::Node& node, const pqxx::row& node_values, int experiment_run_id);
    void FillContainerState(AggServer::Container& container,
                            const pqxx::row& container_values,
                            int experiment_run_id);
    void FillInterfaceState(AggServer::NetworkInterface& interface,
                            const pqxx::row& interface_values,
                            int experiment_run_id);
    void FillComponentInstanceState(AggServer::ComponentInstance& component_instance,
                                    const pqxx::row& component_instance_values,
                                    int experiment_run_id);
    void FillWorkerInstanceState(AggServer::WorkerInstance& worker_instance,
                                 const pqxx::row& worker_instance_values,
                                 int experiment_run_id);
    void FillPortState(AggServer::Port& port, const pqxx::row& port_values);
    void FillPortConnectionState(AggServer::PortConnection& port_connection,
                                 const DatabaseClient::port_graphml_pair& port_pair,
                                 AggServer::PortConnection::ConnectionType type);

    void RegisterCallbacks();
};

} // namespace re::logging::aggregation::broker

#endif // AGGREGATIONREPLIER_H