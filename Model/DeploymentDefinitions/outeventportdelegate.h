#ifndef OUTEVENTPORTDELEGATE_H
#define OUTEVENTPORTDELEGATE_H
#include "eventportdelegate.h"

class OutEventPortDelegate: public EventPortAssembly
{
    Q_OBJECT
public:
    OutEventPortDelegate();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};


#endif // OUTEVENTPORTDELEGATE_H
