#include "attributeinstance.h"

namespace re::Representation {

auto ConstructAttributeInstance(const AttributeInstancePb& attr_pb)
    -> std::unique_ptr<AttributeInstanceInterface>
{
    types::Uuid definition_uuid{attr_pb.definition_uuid()};
    switch(attr_pb.type()) {
        case AttributeTypePb::String: {
            return std::make_unique<AttributeInstance<std::string>>(attr_pb.core_data(),
                                                                    definition_uuid, attr_pb.s());
        }
        case AttributeTypePb::Integer: {
            return std::make_unique<AttributeInstance<int64_t>>(attr_pb.core_data(),
                                                                definition_uuid, attr_pb.i());
        }
        case AttributeTypePb::Character: {
            // XXX: This seems horrible. We should probably look at how we're handling character
            //  typed attributes elsewhere also.
            return std::make_unique<AttributeInstance<char>>(attr_pb.core_data(), definition_uuid,
                                                             static_cast<char>(attr_pb.c()));
        }
        case AttributeTypePb::Boolean: {
            // TODO: Investigate if it's worth migrating to protobuf bool type
            return std::make_unique<AttributeInstance<bool>>(attr_pb.core_data(), definition_uuid,
                                                             attr_pb.b());
        }
        case AttributeTypePb::Double: {
            return std::make_unique<AttributeInstance<double>>(attr_pb.core_data(), definition_uuid,
                                                               attr_pb.d());
        }
        case AttributeTypePb::Float: {
            return std::make_unique<AttributeInstance<float>>(attr_pb.core_data(), definition_uuid,
                                                              attr_pb.f());
        }
        case AttributeTypePb::StringList: {
            std::vector<std::string> strings;
            for(const auto& string : attr_pb.string_list()) {
                strings.push_back(string);
            }
            return std::make_unique<AttributeInstance<std::vector<std::string>>>(
                attr_pb.core_data(), definition_uuid, strings);
        }
            // Handle protobuf sentinel values. These cases should never eventuate
        case network::protocol::experimentdefinition::AttributeType_INT_MIN_SENTINEL_DO_NOT_USE_:
            [[fallthrough]];
        case network::protocol::experimentdefinition::AttributeType_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
    throw std::invalid_argument("Invalid type found in attribute in ConstructAttributeInstance");
}

} // namespace re::Representation
