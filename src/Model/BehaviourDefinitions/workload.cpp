#include "workload.h"

#include "../entityfactory.h"

Workload::Workload(EntityFactory* factory) : BehaviourNode(factory, NODE_KIND::WORKLOAD, "Workload"){
	auto node_kind = NODE_KIND::WORKLOAD;
	QString kind_string = "Workload";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Workload();});
};

Workload::Workload():BehaviourNode(NODE_KIND::WORKLOAD){
    setWorkflowReciever(true);
    setWorkflowProducer(true);
    setExpandEnabled(true);
}

bool Workload::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::PROCESS:
    case NODE_KIND::WORKER_PROCESS:
        break;
    default:
        return false;
    }
    return BehaviourNode::canAdoptChild(child);
}

bool Workload::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
