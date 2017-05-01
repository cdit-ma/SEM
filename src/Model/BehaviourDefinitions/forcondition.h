#ifndef FORCONDITION_H
#define FORCONDITION_H
#include "condition.h"

class EntityFactory;
class ForCondition: public Condition
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ForCondition(EntityFactory* factory);
	ForCondition();
public:
    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // FORCONDITION_H
