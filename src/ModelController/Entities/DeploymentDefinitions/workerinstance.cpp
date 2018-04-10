#include "workerinstance.h"

const NODE_KIND node_kind = NODE_KIND::WORKER_INSTANCE;
const QString kind_string = "WorkerInstance";


MEDEA::WorkerInstance::WorkerInstance(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WorkerInstance();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
};

MEDEA::WorkerInstance::WorkerInstance() : Node(NODE_KIND::WORKER_INSTANCE)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    //setNodeType(NODE_TYPE::DEFINITION);

    setInstanceKind(node_kind);
    //setImplKind(node_kind);

    setDefinitionKind(NODE_KIND::WORKER_DEFINITION);
}

bool MEDEA::WorkerInstance::canAdoptChild(Node* node)
{
    switch(node->getNodeKind()){
        case NODE_KIND::ATTRIBUTE_INSTANCE:
            break;
        default:
            return false;
    }
    return Node::canAdoptChild(node);
}

bool MEDEA::WorkerInstance::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        switch(dst->getNodeKind()){
            case NODE_KIND::WORKER_DEFINITION:{
                return true;
            }
            /*case NODE_KIND::WORKER_INSTANCE:{
                return true;
            }*/
            default:
                return false;
        }
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}
