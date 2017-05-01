#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "eventportimpl.h"

class EntityFactory;
class InEventPortImpl : public EventPortImpl
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InEventPortImpl(EntityFactory* factory);
	InEventPortImpl();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // INEVENTPORT_H
