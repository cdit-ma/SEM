#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "eventportimpl.h"

class InEventPortImpl : public EventPortImpl
{
    Q_OBJECT
public:
    InEventPortImpl();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // INEVENTPORT_H
