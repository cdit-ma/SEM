#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "../node.h"

class EntityFactory;
class InEventPortImpl : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InEventPortImpl(EntityFactory* factory);
	InEventPortImpl();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};


#endif // INEVENTPORT_H
