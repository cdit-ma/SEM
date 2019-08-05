#ifndef AGGREGATIONREPLIER_H
#define AGGREGATIONREPLIER_H

#include <zmq/protoreplier/protoreplier.hpp>
#include "../databaseclient.h"

#include <proto/aggregationmessage/aggregationmessage.pb.h>


namespace AggServer {

class AggregationReplier : public zmq::ProtoReplier {
public:
    AggregationReplier(std::shared_ptr<DatabaseClient> db_client);

    std::unique_ptr<AggServer::ExperimentRunResponse>
    ProcessExperimentRunRequest(
        const AggServer::ExperimentRunRequest& message
    );

    std::unique_ptr<AggServer::ExperimentStateResponse>
    ProcessExperimentStateRequest(
        const AggServer::ExperimentStateRequest& message
    );

    std::unique_ptr<AggServer::PortLifecycleResponse>
    ProcessPortLifecycleRequest(
        const AggServer::PortLifecycleRequest& message
    );

    std::unique_ptr<AggServer::WorkloadResponse>
    ProcessWorkloadEventRequest(
        const AggServer::WorkloadRequest& message
    );

    std::unique_ptr<AggServer::MarkerResponse>
    ProcessMarkerRequest(
        const AggServer::MarkerRequest& message
    );

    std::unique_ptr<AggServer::CPUUtilisationResponse>
    ProcessCPUUtilisationRequest(
        const AggServer::CPUUtilisationRequest& message
    );

    std::unique_ptr<AggServer::MemoryUtilisationResponse>
    ProcessMemoryUtilisationRequest(
        const AggServer::MemoryUtilisationRequest& message
    );

private:
    std::shared_ptr<DatabaseClient> database_;

    void FillNodeState(
        AggServer::Node& node,
        const pqxx::row& node_values,
        int experiment_run_id
    );
    void FillContainerState(
        AggServer::Container& container,
        const pqxx::row& container_values,
        int experiment_run_id
    );
    void FillComponentInstanceState(
        AggServer::ComponentInstance& component_instance,
        const pqxx::row& component_instance_values,
        int experiment_run_id
    );
    void FillWorkerInstanceState(
        AggServer::WorkerInstance& worker_instance,
        const pqxx::row& worker_instance_values,
        int experiment_run_id
        );
    void FillPortState(AggServer::Port& port, const pqxx::row& port_values);

    void RegisterCallbacks();
};

}

#endif //AGGREGATIONREPLIER_H