#ifndef VECTOR_H
#define VECTOR_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class Vector : public DataNode
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Vector(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    static void updateVectorIcon(Node* node);

protected:
    void childAdded(Node* child);
    void childRemoved(Node* child);
};


#endif // AGGREGATE_H

