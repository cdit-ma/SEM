#ifndef IDL_H
#define IDL_H
#include "../node.h"

class EntityFactory;
class IDL : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	IDL(EntityFactory* factory);
	IDL();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // IDL_H
