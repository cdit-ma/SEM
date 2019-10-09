#include "aggregationrequester.h"

/**
 * @brief AggServer::Requester::Requester
 * @param aggregation_broker_endpoint
 */
AggServer::Requester::Requester(const std::string& aggregation_broker_endpoint):
    requester_(aggregation_broker_endpoint)
{
}


/**
 * @brief AggServer::Requester::GetExperimentRuns
 * @param request
 * @return
 */
std::unique_ptr<AggServer::ExperimentRunResponse> AggServer::Requester::GetExperimentRuns(const AggServer::ExperimentRunRequest &request)
{
    auto reply = requester_.SendRequest<ExperimentRunRequest, ExperimentRunResponse>("GetExperimentRuns", request, 5000);
    try{
        return reply.get();
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }
}


/**
 * @brief AggServer::Requester::GetExperimentState
 * @param request
 * @return
 */
std::unique_ptr<AggServer::ExperimentStateResponse> AggServer::Requester::GetExperimentState(const AggServer::ExperimentStateRequest &request)
{
    auto reply = requester_.SendRequest<ExperimentStateRequest, ExperimentStateResponse>("GetExperimentState", request, 5000);
    try{
        return reply.get();
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }
}


/**
 * @brief AggServer::Requester::GetPortLifecycle
 * @param request
 * @return
 */
std::unique_ptr<AggServer::PortLifecycleResponse> AggServer::Requester::GetPortLifecycle(const AggServer::PortLifecycleRequest& request)
{
    auto reply = requester_.SendRequest<PortLifecycleRequest, PortLifecycleResponse>("GetPortLifecycle", request, 5000);
    try{
        return reply.get();
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }
}


/**
 * @brief AggServer::Requester::GetWorkload
 * @param request
 * @return
 */
std::unique_ptr<AggServer::WorkloadResponse> AggServer::Requester::GetWorkload(const AggServer::WorkloadRequest &request)
{
    auto reply = requester_.SendRequest<WorkloadRequest, WorkloadResponse>("GetWorkload", request, 5000);
    try{
        return reply.get();
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }
}


/**
 * @brief AggServer::Requester::GetCPUUtilisation
 * @param request
 * @return
 */
std::unique_ptr<AggServer::CPUUtilisationResponse> AggServer::Requester::GetCPUUtilisation(const AggServer::CPUUtilisationRequest &request)
{
    auto reply = requester_.SendRequest<CPUUtilisationRequest, CPUUtilisationResponse>("GetCPUUtilisation", request, 5000);
    try{
        return reply.get();
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }
}


/**
 * @brief AggServer::Requester::GetMemoryUtilisation
 * @param request
 * @return
 */
std::unique_ptr<AggServer::MemoryUtilisationResponse> AggServer::Requester::GetMemoryUtilisation(const AggServer::MemoryUtilisationRequest &request)
{
    auto reply = requester_.SendRequest<MemoryUtilisationRequest, MemoryUtilisationResponse>("GetMemoryUtilisation", request, 5000);
    try{
        return reply.get();
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }
}


/**
 * @brief AggServer::Requester::GetMarkers
 * @param request
 * @return
 */
std::unique_ptr<AggServer::MarkerResponse> AggServer::Requester::GetMarkers(const AggServer::MarkerRequest &request)
{
    auto reply = requester_.SendRequest<MarkerRequest, MarkerResponse>("GetMarkers", request, 5000);
    try{
        return reply.get();
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }
}
