#ifndef RE_DEFINITION_COMPONENTASSEMBLY_H
#define RE_DEFINITION_COMPONENTASSEMBLY_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include <types/uuid.h>
#include <vector>
namespace re::Representation {

class ComponentAssembly : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::ComponentAssembly;
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    explicit ComponentAssembly(const PbType& pb);

private:
    uint64_t replication_count_;

    std::vector<types::Uuid> component_assembly_uuids_;
    std::vector<types::Uuid> component_instance_uuids_;
    std::vector<types::Uuid> port_delegate_uuids_;
};
} // namespace re::Representation

#endif // RE_COMPONENTASSEMBLY_H
