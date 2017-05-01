#ifndef PROCESS_H
#define PROCESS_H

#include "behaviournode.h"
#include "../nodekinds.h"

class EntityFactory;
class Process: public BehaviourNode
{
    Q_OBJECT
protected:
	Process(EntityFactory* factory, NODE_KIND kind, QString kind_str);
	Process(EntityFactory* factory);
    Process(NODE_KIND kind = NODE_KIND::PROCESS);
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // PROCESS_H
