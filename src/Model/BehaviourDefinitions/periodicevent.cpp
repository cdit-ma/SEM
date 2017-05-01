#include "periodicevent.h"
#include "../nodekinds.h"

#include "../entityfactory.h"

PeriodicEvent::PeriodicEvent(EntityFactory* factory) : BehaviourNode(factory, NODE_KIND::PERIODICEVENT, "PeriodicEvent"){
	auto node_kind = NODE_KIND::PERIODICEVENT;
	QString kind_string = "PeriodicEvent";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new PeriodicEvent();});

    RegisterDefaultData(factory, node_kind, "frequency", QVariant::Double, false, 1);
};

PeriodicEvent::PeriodicEvent():BehaviourNode(NODE_KIND::PERIODICEVENT){
    //Setup initial settings
    setWorkflowProducer(true);
    setWorkflowReciever(false);
}

bool PeriodicEvent::canAdoptChild(Node * node)
{
    Q_UNUSED(node);
    return false;
}

bool PeriodicEvent::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
