
#ifndef RE_COMMON_CONTROL_MESSAGE_HELPER_H
#define RE_COMMON_CONTROL_MESSAGE_HELPER_H

#include <proto/controlmessage/controlmessage.pb.h>
#include <string>
#include <vector>

namespace NodeManager{
    class Attribute;
    typedef ::google::protobuf::Map< ::std::string, ::NodeManager::Attribute > PbAttrMap;

    bool GotAttribute(const PbAttrMap& map, const std::string& attribute_key);
    const NodeManager::Attribute& GetAttribute(const PbAttrMap& map, const std::string& attribute_key);
    NodeManager::Attribute& InsertAttribute(PbAttrMap* map, const std::string& attribute_key, const NodeManager::Attribute::Kind kind);
    NodeManager::Attribute& AddAllocatedAttribute(PbAttrMap* map, Attribute* attribute);

    NodeManager::Attribute& SetStringAttribute(PbAttrMap* map, const std::string& attribute_key, const std::string& value);
    NodeManager::Attribute& SetStringListAttribute(PbAttrMap* map, const std::string& attribute_key, const std::vector<std::string>& value);
    NodeManager::Attribute& SetDoubleAttribute(PbAttrMap* map, const std::string& attribute_key, const double value);
    NodeManager::Attribute& SetIntegerAttribute(PbAttrMap* map, const std::string& attribute_key, const int value);
    NodeManager::Attribute& SetBooleanAttribute(PbAttrMap* map, const std::string& attribute_key, const bool value);
};

#endif //RE_COMMON_CONTROL_MESSAGE_HELPER_H