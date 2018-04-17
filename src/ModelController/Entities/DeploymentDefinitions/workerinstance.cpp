#include "workerinstance.h"
#include "../data.h"

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
    setNodeType(NODE_TYPE::DEFINITION);

    setDefinitionKind(NODE_KIND::WORKER_DEFINITION);
    setInstanceKind(NODE_KIND::WORKER_INSTANCE);

    SetEdgeRuleActive(Node::EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY, false);
}

bool MEDEA::WorkerInstance::canAdoptChild(Node* node)
{
    switch(node->getNodeKind()){
        case NODE_KIND::ATTRIBUTE_INSTANCE:
            break;
        case NODE_KIND::WORKER_FUNCTION:{
            //Only allow workerInstances to adopt Worker Function
            if(getViewAspect() != VIEW_ASPECT::BEHAVIOUR){
                return false;
            }
            break;
        }
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
            case NODE_KIND::WORKER_DEFINITION:
            case NODE_KIND::WORKER_INSTANCE:{
                if(getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
                    if(dst->getViewAspect() != VIEW_ASPECT::WORKERS) {
                        return false;
                    }
                }
                
                break;
            }
            default:
                return false;
        }
    }
    case EDGE_KIND::DEPLOYMENT:{
        break;
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}


void MEDEA::WorkerInstance::bindWorkerID(Node* child, bool setup){
    if(child){
        auto label_data = getData("label");
        auto worker_id_data = child->getData("workerID");
        if(label_data && worker_id_data){
            label_data->linkData(worker_id_data, setup);
        }
    }
}

void MEDEA::WorkerInstance::childAdded(Node* child){
    bindWorkerID(child, true);
    Node::childAdded(child);
    
}

void MEDEA::WorkerInstance::childRemoved(Node* child){
    bindWorkerID(child, true);
    Node::childRemoved(child);
}


void MEDEA::WorkerInstance::definitionSet(Node* definition){
    auto parent_node = getParentNode();
    
    switch (parent_node->getNodeKind()) {
        case NODE_KIND::COMPONENT_INSTANCE:{
            auto top_definition = getDefinition(true);
            if (top_definition->getDataValue("label") == "OpenCL_Worker") {
                setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, true);
            }
            break;
        }
        default:
            break;
    }
}
