#ifndef MANAGEMENTCOMPONENT_H
#define MANAGEMENTCOMPONENT_H
#include "../node.h"

class EntityFactory;
class ManagementComponent : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ManagementComponent(EntityFactory* factory);
	ManagementComponent();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // MANAGEMENTCOMPONENT_H
