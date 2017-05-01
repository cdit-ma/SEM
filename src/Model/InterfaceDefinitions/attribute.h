#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "../node.h"

class EntityFactory;
class Attribute : public Node
{
    Q_OBJECT
protected:
	Attribute(EntityFactory* factory);
	Attribute();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // ATTRIBUTE_H
