#ifndef RE_NODE_H
#define RE_NODE_H

#include "modelentity.h"
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include "ipv4.hpp"
namespace re::Representation {

class Container;
class LoggingClientDefinition;
class LoggingServerDefinition;
class ComponentInstance;
class ComponentAssembly;
class Node : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::Node;
    explicit Node(const PbType& pb);
    Node(GraphmlParser& parser, const std::string& medea_id);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    auto AddDeployedContainer(const Container& container) -> void;
    auto AddDeployedLoggingClient(const LoggingClientDefinition& logging_client_definition) -> void;
    auto AddDeployedLoggingServer(const LoggingServerDefinition& logging_server_definition) -> void;
    auto AddDeployedComponentInstance(const ComponentInstance& component_instance) -> void;
    auto AddDeployedComponentAssembly(const ComponentAssembly& component_assembly) -> void;

    auto HasDeployedContainer(const Container& container) const -> bool;

    auto GetIpAddress() const -> sem::types::Ipv4;
private:
    sem::types::Ipv4 ip_address_;
    std::vector<sem::types::Uuid> deployed_container_uuids_;
    std::vector<sem::types::Uuid> deployed_component_instance_uuids_;
    std::vector<sem::types::Uuid> deployed_component_assembly_uuids;
    std::vector<sem::types::Uuid> deployed_logging_client_uuids_;
    std::vector<sem::types::Uuid> deployed_logging_server_uuids_;
};

} // namespace re::Representation
#endif // RE_NODE_H
