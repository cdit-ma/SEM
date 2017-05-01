#ifndef COMPONENT_H
#define COMPONENT_H
#include "../node.h"

class EntityFactory;
class Component : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Component(EntityFactory* factory);
	Component();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // COMPONENT_H
