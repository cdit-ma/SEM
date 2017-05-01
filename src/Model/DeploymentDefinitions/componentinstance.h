#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"

class EntityFactory;
class ComponentInstance : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ComponentInstance(EntityFactory* factory);
	ComponentInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // COMPONENTINSTANCE_H
