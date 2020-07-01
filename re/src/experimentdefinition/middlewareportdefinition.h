#ifndef RE_MIDDLEWAREPORTDEFINITION_H
#define RE_MIDDLEWAREPORTDEFINITION_H
#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include <string>
#include "uuid.h"
#include <vector>
namespace re::Representation {

class MiddlewarePortDefinition : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::MiddlewarePortDefinition;
    using MiddlewarePortKindPb = re::network::protocol::experimentdefinition::MiddlewarePortKind;
    explicit MiddlewarePortDefinition(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    enum class Kind { Publisher, Subscriber, Requester, Replier };
    [[nodiscard]] auto GetUuid() -> re::types::Uuid { return GetCoreData().GetUuid(); }

private:
    std::vector<re::types::Uuid> attribute_definition_uuids_;

    // String name of the aggregate we're sending along this port.
    std::string medea_aggregate_type_;

    Kind kind_;
};
} // namespace re::Representation

#endif // RE_MIDDLEWAREPORTDEFINITION_H
