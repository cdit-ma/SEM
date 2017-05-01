#ifndef OUTEVENTPORTIMPL_H
#define OUTEVENTPORTIMPL_H
#include "eventportimpl.h"

class EntityFactory;
class OutEventPortImpl : public EventPortImpl
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
