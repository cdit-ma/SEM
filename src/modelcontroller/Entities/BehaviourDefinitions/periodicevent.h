#ifndef PERIODICEVENT_H
#define PERIODICEVENT_H
#include "containernode.h"

class EntityFactory;
class PeriodicEvent: public Node, public ContainerNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
	PeriodicEvent(EntityFactory& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // PERIODICEVENT_H

