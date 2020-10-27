#ifndef RE_NODEMANAGER_H
#define RE_NODEMANAGER_H

#include "controlmessage.pb.h"
#include "epmregistrarimpl.h"
#include "epmregistration.pb.h"
#include "epmregistry.h"
#include "experimentprocessmanager.h"
#include "grpc_util/server.h"
#include "nodemanagerconfig.h"
#include "nodemanagercontrolimpl.h"
#include "replier.hpp"
#include "requester.hpp"
#include <boost/process/child.hpp>
#include <memory>
namespace sem::node_manager {

class NodeManager {
public:
    explicit NodeManager(NodeConfig config, EpmRegistry& epm_registry);
    ~NodeManager();
    auto wait() -> void;
    auto shutdown() -> void;

private:
    // Data member order matters as we initialise most data members in class' init list
    const NodeConfig node_config_;
    EpmRegistry& epm_registry_;

    std::shared_ptr<EpmRegistrarImpl> epm_registration_service_;
    std::shared_ptr<NodeManagerControlImpl> control_service_;

    sem::grpc_util::Server server_;
};

} // namespace sem::node_manager

#endif // RE_NODEMANAGER_H
