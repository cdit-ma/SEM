#ifndef VECTOR_H
#define VECTOR_H
#include "datanode.h"

class EntityFactory;
class Vector : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    Vector(EntityFactory& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    static void updateVectorIcon(Node* node);
protected:
    void childAdded(Node* child);
    void childRemoved(Node* child);
};


#endif // AGGREGATE_H

