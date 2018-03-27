#include "workerdefinition.h"



WorkerDefinition::WorkerDefinition(EntityFactory* factory) : Node(factory, NODE_KIND::WORKER_DEFINITION, "WorkerDefinition"){
	auto node_kind = NODE_KIND::WORKER_DEFINITION;
	QString kind_string = "WorkerDefinition";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WorkerDefinition();});
};

WorkerDefinition::WorkerDefinition():Node(NODE_KIND::WORKER_DEFINITION){
    //setWorkflowReciever(true);
    //setWorkflowProducer(true);
    //setExpandEnabled(true);
}

bool WorkerDefinition::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::ATTRIBUTE:
    case NODE_KIND::WORKER_FUNCTION:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool WorkerDefinition::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
