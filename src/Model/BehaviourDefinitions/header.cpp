#include "header.h"

Header::Header():BehaviourNode(NODE_KIND::HEADER){
    setWorkflowReciever(false);
    setWorkflowProducer(false);

    updateDefaultData("header_name", QVariant::String);
    updateDefaultData("include_path", QVariant::String);
}

bool Header::canAdoptChild(Node *child)
{
    return false;
}

bool Header::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
