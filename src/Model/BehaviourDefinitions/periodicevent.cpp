#include "periodicevent.h"

PeriodicEvent::PeriodicEvent():BehaviourNode(NK_PERIODICEVENT){
    //Setup initial settings
    setWorkflowProducer(true);
    setWorkflowReciever(false);
}

bool PeriodicEvent::canAdoptChild(Node * node)
{
    Q_UNUSED(node);
    return false;
}

bool PeriodicEvent::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
