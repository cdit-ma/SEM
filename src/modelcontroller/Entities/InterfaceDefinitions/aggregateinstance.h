#ifndef AGGREGATEINSTANCE_H
#define AGGREGATEINSTANCE_H
#include "datanode.h"

class EntityFactory;
class AggregateInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	AggregateInstance(EntityFactory* factory);
	AggregateInstance();
    void parentSet(Node* parent);
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
public:
    static void ParentSet(DataNode* child);
};

#endif // AGGREGATEINSTANCE_H
