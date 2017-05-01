#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../node.h"

class EntityFactory;
class AttributeInstance : public Node
{
    Q_OBJECT
protected:
	AttributeInstance(EntityFactory* factory);
	AttributeInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // ATTRIBUTEINSTANCE_H
