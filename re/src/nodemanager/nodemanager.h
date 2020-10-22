#ifndef RE_NODEMANAGER_H
#define RE_NODEMANAGER_H

#include "controlmessage.pb.h"
#include "epmregistration.pb.h"
#include "execution.hpp"
#include "experimentprocessmanager.h"
#include "grpc_util/server.h"
#include "nodemanagerconfig.h"
#include "nodemanagercontrol.pb.h"
#include "replier.hpp"
#include "requester.hpp"
#include <boost/process/child.hpp>
#include <memory>
namespace re::node_manager {

class NodeManager {
public:
    explicit NodeManager(NodeConfig config);
    auto wait() -> void;

private:
    using EpmRegistrationRequest = network::protocol::epmregistration::Request;
    using EpmRegistrationReply = network::protocol::epmregistration::Reply;

    auto
    NewEpm(const sem::types::Uuid& uuid, const std::string& experiment_name) -> sem::types::Uuid;
    auto HandleEpmRegistration(const EpmRegistrationRequest& request) -> EpmRegistrationReply;
    void StopEpm(sem::types::Uuid uuid);
    auto WaitForEpmRegistrationMessage(const sem::types::Uuid& request_uuid) -> sem::types::Uuid;

    const NodeConfig node_config_;
    std::optional<sem::types::SocketAddress> control_endpoint_;

//    sem::grpc_util::Server control_server_;

    std::mutex epm_registration_mutex_;
    std::condition_variable epm_registration_semaphore_;

    struct EpmRegistration {
        sem::types::Uuid request_uuid;
        sem::types::Uuid epm_uuid;
    };

    std::queue<EpmRegistration> epm_registrations_;
    std::unordered_map<sem::types::Uuid, std::unique_ptr<boost::process::child>>
        epm_process_handles_;
};

} // namespace re::NodeManager

#endif // RE_NODEMANAGER_H
