#ifndef BRANCHSTATE_H
#define BRANCHSTATE_H
#include "branch.h"

class BranchState: public Branch
{
    Q_OBJECT
public:
    BranchState();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);

};
#endif // BRANCHSTATE_H
