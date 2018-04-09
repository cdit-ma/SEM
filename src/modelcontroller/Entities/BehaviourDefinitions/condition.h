#ifndef CONDITION_H
#define CONDITION_H
#include "containernode.h"

#include "../../edgekinds.h"

class Termination;
class Branch;

class EntityFactory;
class Condition: public ContainerNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Condition(EntityFactory* factory, NODE_KIND kind, QString kind_str);
	Condition(EntityFactory* factory);

    Condition(NODE_KIND kind = NODE_KIND::CONDITION);
public:
    virtual bool canAdoptChild(Node*);
    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // CONDITION_H
