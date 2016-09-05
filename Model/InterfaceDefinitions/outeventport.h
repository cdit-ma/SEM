#ifndef OUTEVENTPORT_H
#define OUTEVENTPORT_H
#include "eventport.h"

class OutEventPort : public EventPort
{
    Q_OBJECT
public:
    OutEventPort();
    bool canAdoptChild(Node *node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // OUTEVENTPORT_H
