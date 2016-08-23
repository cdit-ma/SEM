#ifndef PROCESS_H
#define PROCESS_H
#include "behaviournode.h"

class Process: public BehaviourNode
{
    Q_OBJECT
public:
    Process();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};


#endif // PROCESS_H
