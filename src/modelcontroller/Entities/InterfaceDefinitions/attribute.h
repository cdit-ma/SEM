#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "../node.h"

class EntityFactory;
class Attribute : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Attribute(EntityFactory* factory);
	Attribute();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // ATTRIBUTE_H
