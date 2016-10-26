#ifndef PROCESS_H
#define PROCESS_H
#include "behaviournode.h"

class Process: public BehaviourNode
{
    Q_OBJECT
public:
    Process(Node::NODE_KIND kind = Node::NK_PROCESS);
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // PROCESS_H
