#include "branch.h"

Branch::Branch():BehaviourNode(){}

Termination *Branch::getTermination()
{
    foreach(Edge* edge, getEdges(0)){
        if(edge->getSource() == this && edge->getDestination()->getNodeKind() == "Termination"){
            return (Termination*) edge->getDestination();
        }
    }
    return 0;
}

bool Branch::canAdoptChild(Node *node)
{
    return BehaviourNode::canAdoptChild(node);
}

bool Branch::canConnect(Node *node)
{
    Termination* terminate = dynamic_cast<Termination*>(node);

    //Can't connect to a Termination which has a Branch attacted.
    if(terminate && terminate->getBranch()){
        return false;
    }

    //Can't connect to a Termination, if I already have a Termination()
    if(terminate && getTermination()){
        return false;
    }

    return BehaviourNode::canConnect(node);
}

