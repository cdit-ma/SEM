#include "process.h"
#include "parameter.h"
#include "../data.h"
Process::Process(NODE_KIND kind):BehaviourNode(kind){
    setWorkflowReciever(true);
    setWorkflowProducer(true);
    //setWorkflowProducer(false);
    //setWorkflowReciever(false);
}

bool Process::canAdoptChild(Node* node)
{
    if(!node->isNodeOfType(NT_PARAMETER)){
        return false;
    }

    Parameter* parameter = (Parameter*)node;

    if(parameter->isReturnParameter()){
        if(!getChildrenOfKind(NK_RETURN_PARAMETER, 0).isEmpty()){
            return false;
        }
    }
    if(parameter->isVariadicParameter()){
        //Check to see if worker function is variadic
        auto d = getData("is_variadic");
        if(d && !d->getValue().toBool()){
            //If we are variadic
            return false;
        }
    }

    return BehaviourNode::canAdoptChild(node);
}

bool Process::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
    //Q_UNUSED(edgeKind);
    //Q_UNUSED(dst);
    //return false;
}
