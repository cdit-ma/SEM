#include "termination.h"
#include "branch.h"
Termination::Termination():BehaviourNode(){

}
Termination::~Termination(){

}

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

bool Termination::canAdoptChild(Node*)
{
    return false;
}
