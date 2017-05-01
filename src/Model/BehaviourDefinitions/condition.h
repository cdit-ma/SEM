#ifndef CONDITION_H
#define CONDITION_H
#include "behaviournode.h"
#include "../nodekinds.h"

class Termination;
class Branch;

class EntityFactory;
class Condition: public BehaviourNode
{
    Q_OBJECT
protected:
	Condition(EntityFactory* factory, NODE_KIND kind, QString kind_str);
	Condition(EntityFactory* factory);

    Condition(NODE_KIND kind = NODE_KIND::CONDITION);
public:

    Branch* getBranch();
    Termination* getRequiredTermination();
    bool gotTermination();

    virtual bool canAdoptChild(Node*);
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // CONDITION_H
