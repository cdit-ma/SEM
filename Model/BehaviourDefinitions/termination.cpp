#include "termination.h"
#include "branch.h"
#include <QDebug>
Termination::Termination():BehaviourNode(NK_TERMINATION){
    setWorkflowProducer(true);
    setWorkflowReciever(true);
}


Branch *Termination::getBranch()
{
    foreach(Edge* edge, getEdges(0, Edge::EC_WORKFLOW)){
        BehaviourNode* source = (BehaviourNode*) edge->getSource();
        if(source->isNodeOfType(NT_BRANCH)){
            Branch* branch = (Branch*) source;
            if(branch->getTermination() == this){
                return branch;
            }
        }
    }
    return 0;
}

bool Termination::canAdoptChild(Node*)
{
    return false;
}

bool Termination::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_WORKFLOW:{
        if(!getBranch()){
            qCritical() << "NO BRANCH";
            return false;
        }
        break;
    }
    default:
        break;
    }
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}

