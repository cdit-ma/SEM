#ifndef AGGSERVER_REQUESTER_H
#define AGGSERVER_REQUESTER_H

#include <string>
#include <memory>

#include <proto/aggregationmessage/aggregationmessage.pb.h>
#include <zmq/protorequester/protorequester.hpp>

namespace AggServer
{
class Requester
{
public:
    Requester(const std::string& aggregation_broker_endpoint);
    std::unique_ptr<AggServer::ExperimentRunResponse> GetExperimentRuns(const AggServer::ExperimentRunRequest& request);
    std::unique_ptr<AggServer::ExperimentStateResponse> GetExperimentState(const AggServer::ExperimentStateRequest& request);
    std::unique_ptr<AggServer::PortLifecycleResponse> GetPortLifecycle(const AggServer::PortLifecycleRequest& request);
    std::unique_ptr<AggServer::WorkloadResponse> GetWorkload(const AggServer::WorkloadRequest& request);
    std::unique_ptr<AggServer::CPUUtilisationResponse> GetCPUUtilisation(const AggServer::CPUUtilisationRequest& request);
    std::unique_ptr<AggServer::MemoryUtilisationResponse> GetMemoryUtilisation(const AggServer::MemoryUtilisationRequest& request);
    std::unique_ptr<AggServer::MarkerResponse> GetMarkers(const AggServer::MarkerRequest& request);

private:
    zmq::ProtoRequester requester_;
};
}

#endif //AGGSERVER_REQUESTER_H
