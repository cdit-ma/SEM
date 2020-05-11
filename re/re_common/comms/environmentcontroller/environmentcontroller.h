#ifndef ENVIRONMENTMANAGER_ENVIRONMENTCONTROLLER_H
#define ENVIRONMENTMANAGER_ENVIRONMENTCONTROLLER_H

#include <string>
#include <vector>

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
#include "experimentdefinition.pb.h"
#include "controlmessage.pb.h"
#ifdef _WIN32
#pragma warning(pop)
#endif
#include "protorequester.hpp"
#include "uuid.h"

namespace EnvironmentManager {
class EnvironmentController {
public:
    EnvironmentController(const std::string& environment_manager_endpoint);

    std::vector<std::string> ShutdownExperiment(const std::string& experiment_name, bool is_regex);

    std::unique_ptr<re::network::protocol::experimentdefinition::RegistrationReply>
    AddExperiment(const std::string& experiment_name, const std::string& graphml_path, int duration);

    auto StartExperiment(re::types::Uuid experiment_uuid) -> bool;

    std::vector<std::string> ListExperiments();

    std::string InspectExperiment(const std::string& experiment_name);

    std::string GetQpidBrokerEndpoint();
    std::string GetTaoCosnamingBrokerEndpoint();

private:
    zmq::ProtoRequester requester_;
    const std::chrono::milliseconds standard_timeout{3000};
};
}; // namespace EnvironmentManager

#endif // ENVIRONMENTMANAGER_ENVIRONMENTCONTROLLER_H
