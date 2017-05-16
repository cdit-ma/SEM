#ifndef RETURNPARAMETER_H
#define RETURNPARAMETER_H
#include "parameter.h"

class EntityFactory;
class ReturnParameter : public Parameter
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ReturnParameter(EntityFactory* factory);
	ReturnParameter();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // RETURNPARAMETER_H
