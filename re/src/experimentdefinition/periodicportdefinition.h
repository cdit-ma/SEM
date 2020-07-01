#ifndef RE_DEFINITION_PORT_H
#define RE_DEFINITION_PORT_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include <string>
#include "uuid.h"
#include <vector>
namespace re::Representation {

// Period of periodic port is stored in one of its attributes.
class PeriodicPortDefinition : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::PeriodicPortDefinition;
    explicit PeriodicPortDefinition(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    types::Uuid frequency_attribute_definition_uuid_;
};

} // namespace re::Representation

#endif // RE_DEFINITION_PORT_H
