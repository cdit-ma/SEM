#ifndef AGGSERVER_REQUESTER_H
#define AGGSERVER_REQUESTER_H

#include <memory>
#include <string>

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
#include "aggregationmessage.pb.h"
#ifdef _WIN32
#pragma warning(pop)
#endif
#include "protorequester.hpp"

namespace AggServer {
class Requester {
public:
    Requester(const std::string& aggregation_broker_endpoint);
    std::unique_ptr<AggServer::ExperimentRunResponse> GetExperimentRuns(const AggServer::ExperimentRunRequest& request);
    std::unique_ptr<AggServer::ExperimentStateResponse> GetExperimentState(const AggServer::ExperimentStateRequest& request);
    std::unique_ptr<AggServer::PortLifecycleResponse> GetPortLifecycle(const AggServer::PortLifecycleRequest& request);
    std::unique_ptr<AggServer::WorkloadResponse> GetWorkload(const AggServer::WorkloadRequest& request);
    std::unique_ptr<AggServer::CPUUtilisationResponse> GetCPUUtilisation(const AggServer::CPUUtilisationRequest& request);
    std::unique_ptr<AggServer::MemoryUtilisationResponse> GetMemoryUtilisation(const AggServer::MemoryUtilisationRequest& request);
    std::unique_ptr<AggServer::MarkerResponse> GetMarkers(const AggServer::MarkerRequest& request);
    std::unique_ptr<AggServer::PortEventResponse> GetPortEvents(const AggServer::PortEventRequest& request);
    std::unique_ptr<AggServer::NetworkUtilisationResponse> GetNetworkUtilisation(const AggServer::NetworkUtilisationRequest& request);
    std::unique_ptr<AggServer::GPUMetricResponse> GetGPUMetric(const AggServer::GPUMetricRequest& request);

private:
    zmq::ProtoRequester requester_;
};
} // namespace AggServer

#endif // AGGSERVER_REQUESTER_H
