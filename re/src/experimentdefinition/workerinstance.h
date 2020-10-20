#ifndef RE_WORKERINSTANCE_H
#define RE_WORKERINSTANCE_H

#include "modelentity.h"
#include "graphmlparser.h"
#include "uuid.h"
namespace re::Representation {

class WorkerDefinition;
class AttributeInstanceInterface;
class WorkerInstance : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::WorkerInstance;
    explicit WorkerInstance(const PbType& pb);
    WorkerInstance(GraphmlParser& parser,
                   const std::string& medea_id,
                   const WorkerDefinition& definition);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    void AddAttributeInstance(const AttributeInstanceInterface& attribute_instance);
    [[nodiscard]] auto GetDefinitionUuid() const -> sem::types::Uuid { return definition_uuid_; }

private:
    sem::types::Uuid definition_uuid_;

    std::vector<sem::types::Uuid> attribute_instance_uuids_;
};
} // namespace re::Representation

#endif // RE_WORKERINSTANCE_H
