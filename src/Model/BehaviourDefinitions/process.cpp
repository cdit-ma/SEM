#include "process.h"
#include "parameter.h"

Process::Process(NODE_KIND kind):BehaviourNode(kind){
    setWorkflowReciever(true);
    setWorkflowProducer(true);
    //setWorkflowProducer(false);
    //setWorkflowReciever(false);
}

bool Process::canAdoptChild(Node* node)
{
    if(!node->isNodeOfType(NODE_TYPE::PARAMETER)){
        return false;
    }

    Parameter* parameter = (Parameter*)node;

    if(parameter->isReturnParameter()){
        if(!getChildrenOfKind(NODE_KIND::RETURN_PARAMETER, 0).isEmpty()){
            return false;
        }
    }
    if(parameter->isVariadicParameter()){
        //Check to see if worker function is variadic
        auto d = gotData("is_variadic");
        if(!d){
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
