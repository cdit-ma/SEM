#include "branchstate.h"
#include "condition.h"

BranchState::BranchState():Branch(NODE_KIND::BRANCH_STATE){
}

bool BranchState::canAdoptChild(Node *child)
{
    if(child->getNodeKind() != NODE_KIND::CONDITION){
        return false;
    }
    return Branch::canAdoptChild(child);
}

bool BranchState::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Branch::canAcceptEdge(edgeKind, dst);
}
