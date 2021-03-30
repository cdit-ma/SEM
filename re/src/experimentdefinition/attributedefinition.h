#ifndef RE_ATTRIBUTEDEFINITION_H
#define RE_ATTRIBUTEDEFINITION_H

#include "modelentity.h"
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include "uuid.h"
namespace re::Representation {

class AttributeDefinition : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::AttributeDefinition;
    explicit AttributeDefinition(const PbType& pb);
    AttributeDefinition(GraphmlParser& parser, const std::string& medea_id);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

    enum class Type { Integer, Character, Boolean, Float, Double, String, StringList };

    [[nodiscard]] auto GetType() const -> Type { return type_; }

private:
    Type type_;
};
} // namespace re::Representation

#endif // RE_ATTRIBUTEDEFINITION_H
