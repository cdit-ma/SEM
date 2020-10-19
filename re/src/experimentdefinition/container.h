#ifndef RE_DEFINITION_CONTAINER_H
#define RE_DEFINITION_CONTAINER_H

#include "loggingclientdefinition.h"
#include "modelentity.h"
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include "uuid.h"
#include <vector>
namespace re::Representation {

class Cluster;
class Node;
class ComponentInstance;
class ComponentAssembly;
class Container : public DefaultModelEntity {
public:
    enum class Type { Docker, OsProcess };
    using PbType = re::network::protocol::experimentdefinition::Container;
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    explicit Container(const PbType& pb);
    auto AddDeployedLoggingClient(const LoggingClientDefinition& logging_client) -> void;
    auto AddDeployedComponentAssembly(const ComponentAssembly& component_assembly) -> void;
    auto AddDeployedComponentInstance(const ComponentInstance& component_instance) -> void;

    // Standard container constructor
    Container(GraphmlParser& parser, const std::string& medea_id);

    auto GetType() const -> Type;
    auto GetDeployedComponents() const -> std::vector<types::Uuid>;

    auto Duplicate() -> Container;

private:
    Type container_type_;
    std::vector<types::Uuid> component_instance_uuids_;
    std::vector<types::Uuid> component_assembly_uuids_;
    std::vector<types::Uuid> logger_definition_uuids_;
};
} // namespace re::Representation

#endif // RE_DEFINITION_CONTAINER_H
