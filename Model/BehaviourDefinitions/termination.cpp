#include "termination.h"
#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "branch.h"
#include <QDebug>
Termination::Termination():BehaviourNode(){}
Termination::~Termination(){}

Branch *Termination::getBranch()
{
    foreach(Edge* edge, getEdges(0)){
        Node* node = edge->getSource();
        Branch* branch = dynamic_cast<Branch*>(node);
        if(branch && edge->getDestination() == this){
            return branch;
        }
    }
    return 0;
}

Edge::EDGE_CLASS Termination::canConnect(Node* attachableObject)
{
    return BehaviourNode::canConnect(attachableObject);
}

bool Termination::canAdoptChild(Node*)
{
    return false;
}
