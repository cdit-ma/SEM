#include "process.h"
#include "parameter.h"

Process::Process():BehaviourNode(NK_PROCESS){
    setWorkflowProducer(false);
    setWorkflowReciever(false);
}

bool Process::canAdoptChild(Node* node)
{
    if(!node->isNodeOfType(NT_PARAMETER)){
        return false;
    }

    Parameter* parameter = (Parameter*)node;

    if(parameter->isReturnParameter()){
        if(!getChildrenOfKind(NK_RETURNPARAMETER, 0).isEmpty()){
            return false;
        }
    }
    return BehaviourNode::canAdoptChild(node);
}

bool Process::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
