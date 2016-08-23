#ifndef INEVENTPORTDELEGATE_H
#define INEVENTPORTDELEGATE_H
#include "eventportdelegate.h"

class InEventPortDelegate: public EventPortAssembly
{
    Q_OBJECT
public:
    InEventPortDelegate();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // INEVENTPORTDELEGATE_H
