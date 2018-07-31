#include "helper.h"

bool NodeManager::GotAttribute(const PbAttrMap& map, const std::string& attribute_key){
    return map.count(attribute_key);
};

const NodeManager::Attribute& NodeManager::GetAttribute(const PbAttrMap& map, const std::string& attribute_key){
    if(GotAttribute(map, attribute_key)){
        return map.at(attribute_key);
    }
    throw std::out_of_range("Attribute Map Doesn't contain Key: " + attribute_key);
};

NodeManager::Attribute& NodeManager::AddAllocatedAttribute(PbAttrMap* map, Attribute* attribute){
    auto& attr = (*map)[attribute->info().name()] = * attribute;
    delete attribute;
    return attr;
}

NodeManager::Attribute& NodeManager::InsertAttribute(PbAttrMap* map, const std::string& attribute_key, const NodeManager::Attribute::Kind kind){
    NodeManager::Attribute attribute;
    attribute.mutable_info()->set_name(attribute_key);
    attribute.set_kind(kind);
    return (*map)[attribute_key] = attribute;
}

NodeManager::Attribute& NodeManager::SetStringAttribute(PbAttrMap* map, const std::string& attribute_key, const std::string& value){
    auto& attribute = InsertAttribute(map, attribute_key, NodeManager::Attribute::STRING);
    attribute.add_s(value);
    return attribute;
};

NodeManager::Attribute& NodeManager::SetStringListAttribute(PbAttrMap* map, const std::string& attribute_key, const std::vector<std::string>& value){
    auto& attribute = InsertAttribute(map, attribute_key, NodeManager::Attribute::STRINGLIST);

    for(const auto& str : value){
        attribute.add_s(str);
    }
    return attribute;
};

NodeManager::Attribute& NodeManager::SetDoubleAttribute(PbAttrMap* map, const std::string& attribute_key, const double value){
    auto& attribute = InsertAttribute(map, attribute_key, NodeManager::Attribute::DOUBLE);
    attribute.set_d(value);
    return attribute;
};

NodeManager::Attribute& NodeManager::SetIntegerAttribute(PbAttrMap* map, const std::string& attribute_key, const int value){
    auto& attribute = InsertAttribute(map, attribute_key, NodeManager::Attribute::INTEGER);
    attribute.set_i(value);
    return attribute;
};

NodeManager::Attribute& NodeManager::SetBooleanAttribute(PbAttrMap* map, const std::string& attribute_key, const bool value){
    auto& attribute = InsertAttribute(map, attribute_key, NodeManager::Attribute::BOOLEAN);
    attribute.set_i(value);
    return attribute;
};