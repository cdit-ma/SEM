#ifndef WORKLOAD_H
#define WORKLOAD_H

#include "behaviournode.h"

class EntityFactory;
class Workload: public BehaviourNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Workload(EntityFactory* factory);
	Workload();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // WORKLOAD_H
