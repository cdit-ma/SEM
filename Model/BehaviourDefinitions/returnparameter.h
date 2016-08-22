#ifndef RETURNPARAMETER_H
#define RETURNPARAMETER_H
#include "parameter.h"

class ReturnParameter : public Parameter
{
    Q_OBJECT
public:
    ReturnParameter();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // RETURNPARAMETER_H
