#ifndef ENVIRONMENT_MANAGER_ATTRIBUTE_H
#define ENVIRONMENT_MANAGER_ATTRIBUTE_H

#include <proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Attribute{
    public:
        enum class Kind{
            String = 0,
            Integer = 1,
            Character = 2,
            Boolean = 3,
            Double = 4,
            Float = 5,
            Stringlist = 6,
        };

        Attribute(const NodeManager::Attribute& attribute);
        std::string GetName() const;
        
        std::unique_ptr<NodeManager::Attribute> GetProto(const bool full_update);

    private:
        std::string name_;
        std::string id_;

        Kind kind_;

        std::vector<std::string> string_list_;
        double double_value_;
        int64_t integer_value_;
        bool boolean_value_;

        bool dirty_;

        Attribute::Kind ProtoAttributeKindToInternal(NodeManager::Attribute::Kind kind);
        NodeManager::Attribute::Kind InternalAttributeKindToProto(Attribute::Kind kind);
};
};

#endif //ENVIRONMENT_MANAGER_ATTRIBUTE_H