#include "branch.h"
#include "condition.h"

#include "../entityfactory.h"
Branch::Branch(EntityFactory* factory, NODE_KIND kind, QString kind_str) : BehaviourNode(factory, kind, kind_str){
	
};

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

bool Branch::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case EDGE_KIND::WORKFLOW:{
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

