#ifndef INEVENTPORT_H
#define INEVENTPORT_H
#include "eventport.h"

class InEventPort : public EventPort
{
    Q_OBJECT
public:
    InEventPort();
    bool canAdoptChild(Node *node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // INEVENTPORT_H
