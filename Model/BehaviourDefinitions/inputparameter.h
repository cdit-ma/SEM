#ifndef INPUTPARAMETER_H
#define INPUTPARAMETER_H
#include "parameter.h"

class InputParameter : public Parameter
{
    Q_OBJECT
public:
    InputParameter();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // INPUTPARAMETER_H
