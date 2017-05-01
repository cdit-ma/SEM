#ifndef VARIADICPARAMETER_H
#define VARIADICPARAMETER_H
#include "parameter.h"

class EntityFactory;
class VariadicParameter : public Parameter
{
    Q_OBJECT
protected:
	VariadicParameter(EntityFactory* factory);
	VariadicParameter();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // VARIADICPARAMETER_H
