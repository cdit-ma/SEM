#ifndef VARIABLE_PARAMETER_H
#define VARIABLE_PARAMETER_H
#include "parameter.h"

class VariableParameter : public Parameter
{
    Q_OBJECT
public:
    VariableParameter();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // VARIABLE_PARAMETER_H
