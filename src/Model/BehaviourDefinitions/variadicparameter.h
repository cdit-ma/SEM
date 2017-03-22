#ifndef VARIADICPARAMETER_H
#define VARIADICPARAMETER_H
#include "parameter.h"

class VariadicParameter : public Parameter
{
    Q_OBJECT
public:
    VariadicParameter();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // VARIADICPARAMETER_H
