#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "eventportdelegate.h"

class EntityFactory;
class InEventPortInstance : public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InEventPortInstance(EntityFactory* factory);
	InEventPortInstance();
public:
    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // INEVENTPORTINSTANCE_H
