#ifndef TERMINATION_H
#define TERMINATION_H
#include "behaviournode.h"

class Branch;
class EntityFactory;
class Termination: public BehaviourNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Termination(EntityFactory* factory);
	Termination();
public:
    Branch* getBranch();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // TERMINATION_H
