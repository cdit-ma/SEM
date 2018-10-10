#ifndef ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
#define ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER

#include <thread>
#include <unordered_map>
#include <set>
#include <mutex>
#include <future>
#include "environment.h"
#include "deploymenthandler.h"
#include "aggregationserverhandler.h"

#include <zmq/protoreplier/protoreplier.hpp>
#include <proto/controlmessage/controlmessage.pb.h>
#include <proto/environmentcontrol/environmentcontrol.pb.h>
#include <util/execution.hpp>

class DeploymentRegister{
    public:
        DeploymentRegister(Execution& execution,
                            const std::string& environment_manager_ip_address,
                            const std::string& registration_port,
                            const std::string& qpid_broker_address,
                            const std::string& tao_naming_server_address,
                            int port_range_min = 30000, int port_range_max = 40000);
        ~DeploymentRegister();
    private:
        void Terminate();
        //Re Node Manager Functions
        std::unique_ptr<NodeManager::NodeManagerRegistrationReply> HandleNodeManagerRegistration(const NodeManager::NodeManagerRegistrationRequest& request);
        
        //Logan Managed Server Functions
        std::unique_ptr<NodeManager::LoganRegistrationReply> HandleLoganRegistration(const NodeManager::LoganRegistrationRequest& request);

        //Controller Functions
        std::unique_ptr<NodeManager::RegisterExperimentReply> HandleRegisterExperiment(const NodeManager::RegisterExperimentRequest& request);
        std::unique_ptr<EnvironmentControl::ShutdownExperimentReply> HandleShutdownExperiment(const EnvironmentControl::ShutdownExperimentRequest& message);
        std::unique_ptr<EnvironmentControl::ListExperimentsReply> HandleListExperiments(const EnvironmentControl::ListExperimentsRequest& message);


        //Aggregation Server functions
        std::unique_ptr<NodeManager::AggregationServerRegistrationReply> HandleAggregationServerRegistration(const NodeManager::AggregationServerRegistrationRequest& request);

        //Medea query functions
        std::unique_ptr<NodeManager::MEDEAInterfaceReply> HandleMEDEAInterfaceRequest(const NodeManager::MEDEAInterfaceRequest& message);

        std::unique_ptr<zmq::ProtoReplier> replier_;
        std::unique_ptr<EnvironmentManager::Environment> environment_;


        std::vector<std::unique_ptr<DeploymentHandler> > re_handlers_;
        std::vector<std::unique_ptr<DeploymentHandler> > logan_handlers_;
        std::vector<std::unique_ptr<AggregationServerHandler> > aggregation_server_handlers_;

        Execution& execution_;
        std::string environment_manager_ip_address_;

};

#endif //ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
