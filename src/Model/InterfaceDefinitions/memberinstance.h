#ifndef MEMBERINSTANCE_H
#define MEMBERINSTANCE_H
#include "datanode.h"

class MemberInstance : public DataNode
{
    Q_OBJECT
public:
    MemberInstance();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // MEMBERINSTANCE_H
