#ifndef COMPONENTBEHAVIOUR_H
#define COMPONENTBEHAVIOUR_H
#include "../node.h"

class EntityFactory;
class ComponentImpl: public Node
{
	friend class EntityFactory;
     Q_OBJECT
protected:
	ComponentImpl(EntityFactory* factory);
	ComponentImpl();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // COMPONENTBEHAVIOUR_H


