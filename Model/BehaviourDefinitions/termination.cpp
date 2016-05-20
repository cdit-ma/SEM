#include "termination.h"
#include "branch.h"
#include <QDebug>
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

/**
 * @brief Termination::getLeftWorkflowEdge Overloaded function to return the Branch state of a termination
 * @return
 */
WorkflowEdge* Termination::getLeftWorkflowEdge()
{
    foreach(Edge* edge, getEdges(0)){
        if(edge->getEdgeClass() == Edge::EC_WORKFLOW){
            if(edge->getDestination() == this){
                Node* src = edge->getSource();
                Branch* srcB = dynamic_cast<Branch*>(src);
                if(srcB){
                    //Only return the left most Branch edge
                    return (WorkflowEdge*)edge;
                }
            }
        }
    }
    return 0;
}

bool Termination::canConnect_WorkflowEdge(Node *node)
{
    if(!getBranch()){
        return false;
    }
    return BehaviourNode::canConnect_WorkflowEdge(node);
}
