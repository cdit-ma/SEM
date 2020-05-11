#ifndef RE_DEFINITION_COMPONENTASSEMBLY_H
#define RE_DEFINITION_COMPONENTASSEMBLY_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include "uuid.h"
#include <vector>
#include <graphmlparser/graphmlparser.h>
namespace re::Representation {

class ComponentInstance;
class PortDelegateInstance;
class ComponentAssembly : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::ComponentAssembly;
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    explicit ComponentAssembly(const PbType& pb);
    ComponentAssembly(GraphmlParser& parser, const std::string& medea_id);
    void AddComponentInstance(const ComponentInstance& component_instance);
    void AddComponentAssembly(const ComponentAssembly& component_assembly);
    void AddPortDelegateInstance(const PortDelegateInstance& port_delegate_instance);

private:

    // If our replication count is set locally, use integer representation.
    // Elsewise, store the uuid of the DeplymentAttribute that is linked to our replication.
    std::variant<uint64_t, types::Uuid> replication_;

    std::vector<types::Uuid> component_assembly_uuids_;
    std::vector<types::Uuid> component_instance_uuids_;
    std::vector<types::Uuid> port_delegate_uuids_;
};
} // namespace re::Representation

#endif // RE_COMPONENTASSEMBLY_H
