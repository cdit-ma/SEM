#ifndef RE_CLUSTER_H
#define RE_CLUSTER_H

#include "container.h"
#include "loggingclientdefinition.h"
#include "modelentity.h"
#include "node.h"
#include "graphmlparser.h"
#include "ipv4.hpp"
namespace re::Representation {

class ComponentInstance;
class ComponentAssembly;
class Cluster : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::Cluster;
    explicit Cluster(const PbType& pb);
    Cluster(GraphmlParser& graphml_parser, std::string medea_id);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

    void AddNode(const Node& node);
    void AddDeployedContainer(const Container& container);
    void AddDeployedLoggingClient(const LoggingClientDefinition& logging_client_definition);
    void AddDeployedComponentInstance(const ComponentInstance& component_instance);
    void AddDeployedComponentAssembly(const ComponentAssembly& component_assembly);

    auto HasDeployedContainer(const Container& container) const -> bool;

private:
    std::string jenkins_url_;
    std::vector<sem::types::Uuid> node_uuids_;

    std::vector<sem::types::Uuid> deployed_container_uuids_;
    std::vector<sem::types::Uuid> deployed_component_instance_uuids_;
    std::vector<sem::types::Uuid> deployed_component_assembly_uuids_;
    std::vector<sem::types::Uuid> deployed_logging_client_uuids_;
    std::vector<sem::types::Uuid> deployed_logging_server_uuids_;
};

} // namespace re::Representation
#endif // RE_CLUSTER_H
