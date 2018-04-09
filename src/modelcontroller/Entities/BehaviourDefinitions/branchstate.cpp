#include "branchstate.h"
#include "condition.h"

const NODE_KIND node_kind = NODE_KIND::BRANCH_STATE;
const QString kind_string = "BranchState";


BranchState::BranchState(EntityFactory* factory) : Branch(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new BranchState();});
};

BranchState::BranchState():Branch(node_kind){
}


bool BranchState::canAdoptChild(Node *child)
{
    if(child->getNodeKind() != NODE_KIND::CONDITION){
        return false;
    }
    return Branch::canAdoptChild(child);
}

bool BranchState::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Branch::canAcceptEdge(edgeKind, dst);
}
