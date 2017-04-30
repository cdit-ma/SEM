#ifndef PROCESS_H
#define PROCESS_H

#include "behaviournode.h"
#include "../nodekinds.h"

class Process: public BehaviourNode
{
    Q_OBJECT
public:
    Process(NODE_KIND kind = NODE_KIND::PROCESS);
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // PROCESS_H
