#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../node.h"

class EntityFactory;
class AttributeInstance : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	AttributeInstance(EntityFactory* factory);
	AttributeInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // ATTRIBUTEINSTANCE_H
