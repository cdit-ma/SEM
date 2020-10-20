#ifndef RE_DEFINITION_WORKER_H
#define RE_DEFINITION_WORKER_H

#include "modelentity.h"
#include "graphmlparser.h"
#include <string>
#include "uuid.h"
#include <vector>

namespace re::Representation {
class AttributeDefinition;
class WorkerDefinition : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::WorkerDefinition;
    explicit WorkerDefinition(const PbType& pb);
    WorkerDefinition(GraphmlParser& parser, const std::string& medea_id);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    void AddAttributeDefinition(const AttributeDefinition& attribute_definition);
    [[nodiscard]] auto GetCppClassName() const -> std::string { return cpp_class_name_; }

private:
    std::string cpp_class_name_;
    std::vector<types::Uuid> attribute_definition_uuids_;
};
} // namespace re::Representation
#endif // RE_DEFINITION_WORKER_H
