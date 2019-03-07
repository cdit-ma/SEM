#ifndef ENUM_INSTANCE_H
#define ENUM_INSTANCE_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class EnumInstance : public DataNode
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    EnumInstance(EntityFactoryBroker& factory, bool is_temp_node);
    void parentSet(Node* parent);
public:
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif //ENUM_INSTANCE_H
