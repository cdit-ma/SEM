#ifndef RE_PERIODICPORTINSTANCE_H
#define RE_PERIODICPORTINSTANCE_H

#include "modelentity.h"
#include <chrono>
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include "uuid.h"
#include <vector>

namespace re::Representation {

class PeriodicPortInstance : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::PeriodicPortInstance;
    explicit PeriodicPortInstance(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    types::Uuid definition_uuid_;
    types::Uuid frequency_attribute_uuid_;
};
} // namespace re::Representation

#endif // RE_PERIODICPORTINSTANCE_H
