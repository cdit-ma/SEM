#ifndef PERIODICEVENT_H
#define PERIODICEVENT_H
#include "behaviournode.h"

class EntityFactory;
class PeriodicEvent: public BehaviourNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	PeriodicEvent(EntityFactory* factory);
	PeriodicEvent();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // PERIODICEVENT_H

