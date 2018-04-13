#include "branch.h"
#include "condition.h"


Branch::Branch(EntityFactory* factory, NODE_KIND kind, QString kind_str) : Node(factory, kind, kind_str){
};

Branch::Branch(NODE_KIND kind) : Node(kind){
    setNodeType(NODE_TYPE::BRANCH);
}

bool Branch::canAdoptChild(Node *child)
{
    if(!child->isNodeOfType(NODE_TYPE::CONDITION)){
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Branch::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}

