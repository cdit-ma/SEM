#ifndef OUTEVENTPORTIMPL_H
#define OUTEVENTPORTIMPL_H
#include "../node.h"

class EntityFactory;
class OutEventPortImpl : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OutEventPortImpl(EntityFactory* factory);
	OutEventPortImpl();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // OUTEVENTPORTIMPL_H
