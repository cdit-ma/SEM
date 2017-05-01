#ifndef SETFUNCTION_H
#define SETFUNCTION_H
#include "behaviournode.h"

class EntityFactory;
class Setter: public BehaviourNode
{
    Q_OBJECT
protected:
	Setter(EntityFactory* factory);
	Setter();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif //SETFUNCTION_H
