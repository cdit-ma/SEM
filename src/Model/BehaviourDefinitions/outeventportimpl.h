#ifndef OUTEVENTPORTIMPL_H
#define OUTEVENTPORTIMPL_H
#include "eventportimpl.h"

class OutEventPortImpl : public EventPortImpl
{
    Q_OBJECT
public:
    OutEventPortImpl();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // OUTEVENTPORTIMPL_H
