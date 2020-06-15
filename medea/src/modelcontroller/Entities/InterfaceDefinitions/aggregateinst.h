#ifndef AGGREGATE_INST_H
#define AGGREGATE_INST_H
#include "datanode.h"

class EntityFactoryRegistryBroker;
class AggregateInst : public DataNode
{
    Q_OBJECT
    public:
	    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        AggregateInst(EntityFactoryBroker& factory, bool is_temp_node);
        void parentSet(Node* parent);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
        static void ParentSet(DataNode* child);
};

#endif // AGGREGATE_INST_H
