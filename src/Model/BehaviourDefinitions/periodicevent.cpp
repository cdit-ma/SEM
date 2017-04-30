#include "periodicevent.h"
#include "../nodekinds.h"

PeriodicEvent::PeriodicEvent():BehaviourNode(NODE_KIND::PERIODICEVENT){
    //Setup initial settings
    setWorkflowProducer(true);
    setWorkflowReciever(false);

    updateDefaultData("frequency", QVariant::Double, false, 1);
    //updateDefaultData("type", QVariant::String, false, false, 1);
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
