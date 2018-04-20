#include "workerdefinition.h"

const NODE_KIND node_kind = NODE_KIND::WORKER_DEFINITION;
const QString kind_string = "WorkerDefinition";


WorkerDefinition::WorkerDefinition(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WorkerDefinition();});
};

WorkerDefinition::WorkerDefinition() : Node(node_kind){
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    
    setInstanceKind(NODE_KIND::WORKER_INSTANCE);
    setImplKind(NODE_KIND::WORKER_INSTANCE);
}

bool WorkerDefinition::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::ATTRIBUTE:
    case NODE_KIND::WORKER_FUNCTION:
    case NODE_KIND::EXTERNAL_TYPE:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool WorkerDefinition::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
