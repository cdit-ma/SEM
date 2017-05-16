#ifndef VARIABLE_PARAMETER_H
#define VARIABLE_PARAMETER_H

#include "parameter.h"

class EntityFactory;
class VariableParameter : public Parameter
{
	friend class EntityFactory;
    Q_OBJECT

protected:
	VariableParameter(EntityFactory* factory);
	VariableParameter();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // VARIABLE_PARAMETER_H
