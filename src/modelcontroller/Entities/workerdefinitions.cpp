#include "workerdefinitions.h"
#include "../nodekinds.h"

WorkerDefinitions::WorkerDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::WORKER_DEFINITIONS, "WorkerDefinitions"){
	auto node_kind = NODE_KIND::WORKER_DEFINITIONS;
	QString kind_string = "WorkerDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WorkerDefinitions();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "WORKERS");
};

WorkerDefinitions::WorkerDefinitions(): Node(NODE_KIND::WORKER_DEFINITIONS)
{
    //setAsRoot(1);
    //setMoveEnabled(false);
    //setExpandEnabled(false);
    //updateDefaultData("label", QVariant::String, true, "WORKERS");
}


VIEW_ASPECT WorkerDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::WORKERS;
}

bool WorkerDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::CLASS:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool WorkerDefinitions::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}


