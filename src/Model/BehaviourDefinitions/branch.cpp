#include "branch.h"
#include "condition.h"

Branch::Branch(NODE_KIND kind):BehaviourNode(kind){
    setWorkflowProducer(true);
    setWorkflowReciever(true);
    setNodeType(NODE_TYPE::BRANCH);
}


Termination *Branch::getTermination()
{
    foreach(BehaviourNode* right, getRecieverNodes()){
        if(right->getNodeKind() == NODE_KIND::TERMINATION){
            return (Termination*) right;
        }
    }
    return 0;
}

bool Branch::canAdoptChild(Node *child)
{
    if(!child->isNodeOfType(NODE_TYPE::CONDITION)){
        return false;
    }
    return BehaviourNode::canAdoptChild(child);
}

bool Branch::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_WORKFLOW:{
        if(dst->getNodeKind() != NODE_KIND::TERMINATION){
            return false;
        }
        if(getTermination()){
           //Can't connect to a Termination, if already have a Termination()
           return false;
        }
        break;
    }
    default:
        break;
    }
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}

