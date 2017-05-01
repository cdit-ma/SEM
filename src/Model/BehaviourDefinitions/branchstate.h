#ifndef BRANCHSTATE_H
#define BRANCHSTATE_H
#include "branch.h"

class EntityFactory;
class BranchState: public Branch
{
    Q_OBJECT
protected:
	BranchState(EntityFactory* factory);
	BranchState();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);

};
#endif // BRANCHSTATE_H
