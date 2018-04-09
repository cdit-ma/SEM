#include "workload.h"


const NODE_KIND node_kind = NODE_KIND::WORKLOAD;
const QString kind_string = "Workload";

Workload::Workload(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Workload();});
};

Workload::Workload():Node(node_kind){
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
    return Node::canAdoptChild(child);
}

bool Workload::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
