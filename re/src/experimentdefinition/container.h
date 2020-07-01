#ifndef RE_DEFINITION_CONTAINER_H
#define RE_DEFINITION_CONTAINER_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include "uuid.h"
#include <vector>
namespace re::Representation {

class Container : public DefaultModelEntity {
public:
    enum class Type { Docker, OsProcess };
    using PbType = re::network::protocol::experimentdefinition::Container;
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    explicit Container(const PbType& pb);

private:
    Type container_type_;
    std::vector<types::Uuid> component_instance_uuids_;
    std::vector<types::Uuid> component_assembly_uuids_;
    std::vector<types::Uuid> logger_definition_uuids_;
};
} // namespace re::Representation

#endif // RE_DEFINITION_CONTAINER_H
