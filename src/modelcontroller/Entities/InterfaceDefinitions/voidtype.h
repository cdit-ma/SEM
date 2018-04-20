#ifndef VOIDTYPE_H
#define VOIDTYPE_H
#include "../node.h"

class EntityFactory;
class VoidType : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	VoidType(EntityFactory* factory);
	VoidType();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // VOIDTYPE_H
