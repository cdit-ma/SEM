#ifndef AGGREGATEINSTANCE_H
#define AGGREGATEINSTANCE_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class AggregateInstance : public DataNode
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    AggregateInstance(EntityFactoryBroker& factory, bool is_temp_node);
    void parentSet(Node* parent);
public:
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
public:
    static void ParentSet(DataNode* child);
};

#endif // AGGREGATEINSTANCE_H
