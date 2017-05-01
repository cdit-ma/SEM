#ifndef RETURNPARAMETER_H
#define RETURNPARAMETER_H
#include "parameter.h"

class EntityFactory;
class ReturnParameter : public Parameter
{
    Q_OBJECT
protected:
	ReturnParameter(EntityFactory* factory);
	ReturnParameter();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // RETURNPARAMETER_H
