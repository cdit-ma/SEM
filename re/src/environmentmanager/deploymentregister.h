#ifndef ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
#define ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER

#include "deploymenthandler.h"
#include "environment/environment.h"
#include "experimentmanager.h"
#include "nodemanagerregistry.h"
#include <future>
#include <mutex>
#include <set>
#include <thread>
#include <unordered_map>

#include "experimentdefinition.pb.h"
#include "protoreplier.hpp"
#include "controlmessage.pb.h"
#include "environmentcontrol.pb.h"

// Review (Mitch): Currently a god class. Manages all queries made of the environment manager.

/// External facing class for the environment manager. All queries made of the environment manager
///  run through this class.
namespace re::EnvironmentManager {
class DeploymentRegister {
public:
    DeploymentRegister(const types::Ipv4& environment_manager_ip_address,
                       uint16_t registration_port,
                       const types::SocketAddress& qpid_broker_address,
                       const std::string& tao_naming_server_address,
                       uint16_t port_range_min = 30000,
                       uint16_t port_range_max = 40000);
    ~DeploymentRegister();

private:
    void Terminate();
    void CleanupTask();

    // Logan Managed Server Functions
    std::unique_ptr<NodeManager::LoganRegistrationReply>
    HandleLoganRegistration(const NodeManager::LoganRegistrationRequest& request);

    // Controller Functions
    // Functions invoked by messages sent from the EnvironmentController
    std::unique_ptr<re::network::protocol::experimentdefinition::RegistrationReply>
    HandleRegisterExperiment(const re::network::protocol::experimentdefinition::RegistrationRequest& request);

    std::unique_ptr<EnvironmentControl::ShutdownExperimentReply>
    HandleShutdownExperiment(const EnvironmentControl::ShutdownExperimentRequest& message);

    std::unique_ptr<EnvironmentControl::ListExperimentsReply>
    HandleListExperiments(const EnvironmentControl::ListExperimentsRequest& message);

    std::unique_ptr<NodeManager::InspectExperimentReply>
    HandleInspectExperiment(const NodeManager::InspectExperimentRequest& request);

    // Aggregation Server functions
    std::unique_ptr<NodeManager::AggregationServerRegistrationReply>
    HandleAggregationServerRegistration(
        const NodeManager::AggregationServerRegistrationRequest& request);

    // Test query functions
    // Only used in unit tests...
    // REVIEW (Mitch): This seems like a bad idea
    std::unique_ptr<EnvironmentControl::GetQpidBrokerEndpointReply>
    GetQpidBrokerEndpoint(const EnvironmentControl::GetQpidBrokerEndpointRequest& message);

    std::unique_ptr<EnvironmentControl::GetTaoCosnamingEndpointReply>
    GetTaoCosnamingEndpoint(const EnvironmentControl::GetTaoCosnamingEndpointRequest& message);

    // Medea query functions
    std::unique_ptr<NodeManager::MEDEAInterfaceReply>
    HandleMEDEAInterfaceRequest(const NodeManager::MEDEAInterfaceRequest& message);

    std::unique_ptr<EnvironmentControl::StartExperimentReply>
    HandleStartExperiment(const EnvironmentControl::StartExperimentRequest& request);

    const types::Ipv4 environment_manager_ip_address_;

    NodeManagerRegistry node_manager_registry_;

    std::unique_ptr<zmq::ProtoReplier> replier_;
    std::unique_ptr<Environment> environment_;


    std::mutex handler_mutex_;
    std::vector<std::unique_ptr<DeploymentHandler>> handlers_;

    std::mutex termination_mutex_;
    bool terminated_ = false;

    std::future<void> cleanup_future_;
    std::chrono::seconds cleanup_period_{1};
    std::condition_variable cleanup_cv_;
    std::string FormatLogMessage(const std::vector<std::string>& messages) const;
    void LogError(const std::vector<std::string>& messages) const;
    void Log(const std::vector<std::string>& messages) const;
};

} // namespace re::EnvironmentManager
#endif // ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
