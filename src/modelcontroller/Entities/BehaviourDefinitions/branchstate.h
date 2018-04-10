#ifndef BRANCHSTATE_H
#define BRANCHSTATE_H

#include "branch.h"

class EntityFactory;
class BranchState: public Branch
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	BranchState(EntityFactory* factory);
	BranchState();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);

};
#endif // BRANCHSTATE_H
