#ifndef INPUTPARAMETER_H
#define INPUTPARAMETER_H
#include "parameter.h"

class EntityFactory;
class InputParameter : public Parameter
{
    Q_OBJECT
protected:
	InputParameter(EntityFactory* factory);
	InputParameter();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // INPUTPARAMETER_H
