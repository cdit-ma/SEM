#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "eventportdelegate.h"

class InEventPortInstance : public EventPortAssembly
{
    Q_OBJECT
public:
    InEventPortInstance();

    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // INEVENTPORTINSTANCE_H
