#include "workerdefinitions.h"
#include "../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::WORKER_DEFINITIONS;
const QString kind_string = "WorkerDefinitions";

void WorkerDefinitions::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new WorkerDefinitions(factory, is_temp_node);
        });
}

WorkerDefinitions::WorkerDefinitions(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::CLASS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "WORKERS", true);
}

VIEW_ASPECT WorkerDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::WORKERS;
}