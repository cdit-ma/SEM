#include "termination.h"
#include "branch.h"
Termination::Termination():BehaviourNode(){

}
Termination::~Termination(){

}

Branch *Termination::getBranch()
{
    foreach(Edge* edge, getEdges(0)){
        if(edge->getEdgeClass() == Edge::EC_WORKFLOW){
            Branch* branch = dynamic_cast<Branch*>(edge->getSource());
            if(branch && edge->getDestination() == this){
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

bool Termination::canConnect_WorkflowEdge(Node *node)
{
    if(!getBranch()){
        return false;
    }
    return BehaviourNode::canConnect_WorkflowEdge(node);
}
