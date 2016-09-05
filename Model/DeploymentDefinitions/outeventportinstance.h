#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "eventportdelegate.h"

class OutEventPortInstance : public EventPortAssembly
{
    Q_OBJECT
public:
    OutEventPortInstance();

    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // OUTEVENTPORTINSTANCE_H
