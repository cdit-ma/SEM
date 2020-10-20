#ifndef RE_MIDDLEWAREPORTDEFINITION_H
#define RE_MIDDLEWAREPORTDEFINITION_H
#include "modelentity.h"
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include <string>
#include "uuid.h"
#include <vector>
namespace re::Representation {

class MiddlewarePortDefinition : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::MiddlewarePortDefinition;
    using MiddlewarePortKindPb = re::network::protocol::experimentdefinition::MiddlewarePortKind;
    explicit MiddlewarePortDefinition(const PbType& pb);
    MiddlewarePortDefinition(GraphmlParser& parser, const std::string& medea_id);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    enum class Kind { Publisher, Subscriber, Requester, Replier };

    std::string GetMedeaAggregateType();
    Kind GetKind() { return kind_; }

private:
    std::vector<sem::types::Uuid> attribute_definition_uuids_;

    // String name of the aggregate we're sending along this port.
    std::string medea_aggregate_type_;

    Kind kind_;
};
} // namespace re::Representation

#endif // RE_MIDDLEWAREPORTDEFINITION_H
