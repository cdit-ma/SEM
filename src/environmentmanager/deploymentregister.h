#ifndef ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
#define ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER

#include <thread>
#include <unordered_map>
#include <set>
#include <mutex>
#include <future>
#include "environment.h"
#include "deploymenthandler.h"

#include <zmq/protoreplier/protoreplier.hpp>
#include <proto/controlmessage/controlmessage.pb.h>
#include <proto/environmentcontrol/environmentcontrol.pb.h>
#include <util/execution.hpp>

class DeploymentRegister{
    public:
        DeploymentRegister(Execution& exe, const std::string& ip_addr,
                            const std::string& registration_port,
                            const std::string& qpid_broker_address,
                            const std::string& tao_naming_server_address,
                            int portrange_min = 30000, int portrange_max = 40000);

        void Start();
        void Terminate();

    private:
        //Request handlers
        std::unique_ptr<NodeManager::EnvironmentMessage> HandleAddExperiment(const NodeManager::EnvironmentMessage& message);
        //std::unique_ptr<NodeManager::????> HandleRemoveExperiment(const NodeManager::EnvironmentMessage& message);
        std::unique_ptr<NodeManager::EnvironmentMessage> HandleAddLoganClient(const NodeManager::EnvironmentMessage& message);
        std::unique_ptr<NodeManager::EnvironmentMessage> HandleNodeQuery(const NodeManager::EnvironmentMessage& message);

        std::unique_ptr<EnvironmentControl::ShutdownExperimentReply> HandleShutdownExperiment(const EnvironmentControl::ShutdownExperimentRequest& message);
        std::unique_ptr<EnvironmentControl::ListExperimentsReply> HandleListExperiments(const EnvironmentControl::ListExperimentsRequest& message);

        //Helpers
        std::string TCPify(const std::string& ip_address, const std::string& port) const;
        std::string TCPify(const std::string& ip_address, int port) const;

        //Members
        Execution& execution_;

        std::unique_ptr<zmq::ProtoReplier> replier_;

        std::unique_ptr<EnvironmentManager::Environment> environment_;

        std::string ip_addr_;
        std::string registration_port_;

        std::vector<std::unique_ptr<DeploymentHandler> > deployments_;
        std::vector<std::unique_ptr<DeploymentHandler> > logan_clients_;
};

#endif //ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
