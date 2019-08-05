#ifndef PROTOBUF_MODEL_PARSER_ASSEMBLY_H
#define PROTOBUF_MODEL_PARSER_ASSEMBLY_H


#include <string>
#include <memory>
#include <vector>
#include <list>

class Assembly;
class ComponentReplication {
public:
    ComponentReplication(const std::string& component_instance_id, Assembly* parent = nullptr);

    std::string id_;
    std::string component_instance_id_;

    // Non-owning pointer to parent.
    Assembly* parent_;

    std::list<std::string> GetReplicationLocation() const;
    std::list<int> GetReplicationIndices() const;
    std::string GetUniqueId() const;
    std::string GetUniqueIdSuffix() const;
};

class Assembly {
public:
    Assembly(const std::string& name, const std::string& assembly_id, Assembly* parent = nullptr);
    Assembly(Assembly& assembly);
    Assembly(Assembly&& assembly) = default;

    void SetReplicationCount(const unsigned int replication_count);
    void SetReplicationIndex(unsigned int replication_index);

    void AddChild(std::unique_ptr<Assembly> child);

    Assembly* GetParent() const;
    std::string GetName() const;
    std::string GetAssemblyId() const;
    unsigned int GetReplicationIndex() const;
    unsigned int GetReplicationCount() const;
    static std::string GetUniqueSuffix(const std::vector<int>& indices);

private:

    std::string name_;
    std::string assembly_id_;

    unsigned int replication_count_ = 1;
    unsigned int replication_index_ = 0;

    // Non-owning pointer to parent.
    Assembly* parent_;

    std::vector<std::unique_ptr<Assembly> > children_;

};


#endif //PROTOBUF_MODEL_PARSER_ASSEMBLY_H
