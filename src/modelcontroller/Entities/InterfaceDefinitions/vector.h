#ifndef VECTOR_H
#define VECTOR_H
#include "datanode.h"

class EntityFactory;
class Vector : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Vector(EntityFactory* factory);
	Vector();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
protected:
    void childAdded(Node* child);
    void childRemoved(Node* child);
};


#endif // AGGREGATE_H

