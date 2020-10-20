#include "attributeinstance.h"

namespace re::Representation {

auto ConstructAttributeInstance(const AttributeInstancePb& attr_pb)
    -> std::unique_ptr<AttributeInstanceInterface>
{
    sem::types::Uuid definition_uuid{attr_pb.definition_uuid()};
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

auto ConstructAttributeInstance(GraphmlParser& parser,
                                const std::string& medea_id,
                                const sem::types::Uuid& definition_uuid)
    -> std::unique_ptr<AttributeInstanceInterface>
{
    CoreData core_data{sem::types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")};

    auto type_str = parser.GetDataValue(medea_id, "type");
    auto value_str = parser.GetDataValue(medea_id, "value");

    if(type_str == "String") {
        return std::make_unique<AttributeInstance<std::string>>(core_data, definition_uuid,
                                                                value_str);
    } else if(type_str == "Integer") {
        int64_t value = 0;
        try {
            value = std::stoi(value_str);
        } catch(const std::invalid_argument& ex) {
            std::cerr << "No value assigned to attribute with id: " << medea_id
                      << " using default of 0" << std::endl;
        }
        return std::make_unique<AttributeInstance<int64_t>>(core_data, definition_uuid, value);

    } else if(type_str == "Character") {
        // XXX: This seems horrible. We should probably look at how we're handling character
        //  typed attributes elsewhere also.
        return std::make_unique<AttributeInstance<char>>(core_data, definition_uuid,
                                                         value_str.front());
    } else if(type_str == "Boolean") {
        return std::make_unique<AttributeInstance<bool>>(core_data, definition_uuid,
                                                         value_str == "true");
    } else if(type_str == "Double") {
        double value = 0;
        try {
            value = std::stod(value_str);
        } catch(const std::invalid_argument& ex) {
            std::cerr << "No value assigned to attribute with id: " << medea_id
                      << " using default of 0" << std::endl;
        }
        return std::make_unique<AttributeInstance<double>>(core_data, definition_uuid, value);

    } else if(type_str == "Float") {
        float value = 0;
        try {
            value = std::stof(value_str);
        } catch(const std::invalid_argument& ex) {
            std::cerr << "No value assigned to attribute with id: " << medea_id
                      << " using default of 0" << std::endl;
        }
        return std::make_unique<AttributeInstance<float>>(core_data, definition_uuid, value);

    } else if(type_str == "StringList") {
        // TODO: Find out what we split on for string list attributes.
        // XXX: Currently no way to add string lists in MEDEA?
        std::vector<std::string> strings;
        //        for(const auto& string : ) {
        //            strings.push_back(string);
        //        }
        return std::make_unique<AttributeInstance<std::vector<std::string>>>(core_data,
                                                                             definition_uuid,
                                                                             strings);
    } else {
        throw std::invalid_argument("Invalid type found in attribute in "
                                    "ConstructAttributeInstance");
    }
    throw std::invalid_argument("Invalid type found in attribute in ConstructAttributeInstance");
}
} // namespace re::Representation
