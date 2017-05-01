#include "branchstate.h"
#include "condition.h"

#include "../entityfactory.h"

BranchState::BranchState(EntityFactory* factory) : Branch(factory, NODE_KIND::BRANCH_STATE, "BranchState"){
	auto node_kind = NODE_KIND::BRANCH_STATE;
	QString kind_string = "BranchState";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new BranchState();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "condition", QVariant::String);
};

BranchState::BranchState():Branch(NODE_KIND::BRANCH_STATE){
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
