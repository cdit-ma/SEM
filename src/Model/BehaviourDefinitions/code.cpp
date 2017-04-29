#include "code.h"

Code::Code():BehaviourNode(NK_CODE){
    setWorkflowReciever(true);
    setWorkflowProducer(true);

    updateDefaultData("code", QVariant::String);
}

bool Code::canAdoptChild(Node *child)
{
    return false;
}

bool Code::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
