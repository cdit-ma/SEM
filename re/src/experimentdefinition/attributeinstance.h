#ifndef RE_DEFINITION_ATTRIBUTEINSTANCE_H
#define RE_DEFINITION_ATTRIBUTEINSTANCE_H

#include "attributedefinition.h"
#include "modelentity.h"
#include "graphmlparser.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include <string>
#include "uuid.h"
#include <utility>
#include <variant>
#include <vector>

namespace re::Representation {

using AttributeInstancePb = re::network::protocol::experimentdefinition::AttributeInstance;
using AttributeTypePb = re::network::protocol::experimentdefinition::AttributeType;
// Pure virtual Attribute interface.
class AttributeInstanceInterface : public ModelEntity {
public:
    [[nodiscard]] virtual auto ToProto() const -> std::unique_ptr<AttributeInstancePb> = 0;

    //[[nodiscard]] virtual auto GetDefinition() const -> AttributeDefinition = 0;

    template<typename AttributeType>[[nodiscard]] auto As() -> AttributeType;
};

// Convenience constructor
auto ConstructAttributeInstance(const AttributeInstancePb& attr_pb)
    -> std::unique_ptr<AttributeInstanceInterface>;

auto ConstructAttributeInstance(GraphmlParser& parser,
                                const std::string& medea_id,
                                const types::Uuid& definition_uuid)
    -> std::unique_ptr<AttributeInstanceInterface>;

template<typename ValueType> class AttributeInstance final : public AttributeInstanceInterface {
public:
    AttributeInstance(const CoreData::PbType& core_data_pb,
                      types::Uuid definition_uuid,
                      const ValueType& value) :
        core_data_{core_data_pb}, definition_uuid_{definition_uuid}, value_{value}
    {
    }

    AttributeInstance(CoreData core_data, types::Uuid definition_uuid, const ValueType& value) :
        core_data_{std::move(core_data)}, definition_uuid_{definition_uuid}, value_{value}
    {
    }

    [[nodiscard]] auto ToProto() const -> std::unique_ptr<AttributeInstancePb> final;

    [[nodiscard]] auto GetCoreData() const -> CoreData final { return core_data_; };

    //[[nodiscard]] auto GetDefinition() const -> AttributeDefinition final
    //{
    //};

    auto SetCoreData(const CoreData& core_data) -> void { core_data_ = core_data; };

    [[nodiscard]] auto Value() -> ValueType { return value_; }

private:
    CoreData core_data_;
    types::Uuid definition_uuid_;
    ValueType value_;
    [[nodiscard]] auto GetPrefilledProto() const -> std::unique_ptr<AttributeInstancePb>
    {
        auto out = std::make_unique<AttributeInstancePb>();
        out->set_allocated_core_data(GetCoreData().ToProto().release());
        out->set_definition_uuid(definition_uuid_.to_string());
        return out;
    };
};

template<>
inline auto AttributeInstance<int64_t>::ToProto() const -> std::unique_ptr<AttributeInstancePb>
{
    auto out = GetPrefilledProto();
    out->set_type(AttributeTypePb::Integer);
    out->set_i(value_);
    return out;
}

template<>
inline auto AttributeInstance<double>::ToProto() const -> std::unique_ptr<AttributeInstancePb>
{
    auto out = GetPrefilledProto();
    out->set_type(AttributeTypePb::Double);
    out->set_d(value_);
    return out;
}

template<>
inline auto AttributeInstance<std::string>::ToProto() const -> std::unique_ptr<AttributeInstancePb>
{
    auto out = GetPrefilledProto();
    out->set_type(AttributeTypePb::String);
    out->set_s(value_);
    return out;
}

template<>
inline auto
AttributeInstance<std::vector<std::string>>::ToProto() const -> std::unique_ptr<AttributeInstancePb>
{
    auto out = GetPrefilledProto();
    out->set_type(AttributeTypePb::StringList);
    for(const auto& str : value_) {
        out->add_string_list(str);
    }
    return out;
}

template<>
inline auto AttributeInstance<bool>::ToProto() const -> std::unique_ptr<AttributeInstancePb>
{
    auto out = GetPrefilledProto();
    out->set_type(AttributeTypePb::Boolean);
    out->set_b(value_);
    return out;
}

template<>
inline auto AttributeInstance<float>::ToProto() const -> std::unique_ptr<AttributeInstancePb>
{
    auto out = GetPrefilledProto();
    out->set_type(AttributeTypePb::Float);
    out->set_f(value_);
    return out;
}

template<>
inline auto AttributeInstance<char>::ToProto() const -> std::unique_ptr<AttributeInstancePb>
{
    auto out = GetPrefilledProto();
    out->set_type(AttributeTypePb::Character);
    out->set_c(value_);
    return out;
}

template<typename T> auto AttributeInstanceInterface::As() -> T
{
    return static_cast<T>(this);
}

} // namespace re::Representation

#endif // RE_DEFINITION_ATTRIBUTE_H
