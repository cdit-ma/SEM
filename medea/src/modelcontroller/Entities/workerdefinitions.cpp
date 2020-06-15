#include "workerdefinitions.h"
#include "../entityfactorybroker.h"
#include "../entityfactoryregistrybroker.h"
#include "../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::WORKER_DEFINITIONS;
const QString kind_string = "Worker Definitions";

void WorkerDefinitions::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new WorkerDefinitions(broker, is_temp_node);
        });
}

WorkerDefinitions::WorkerDefinitions(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::CLASS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    setLabelFunctional(false);
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "WORKERS");
}

VIEW_ASPECT WorkerDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::WORKERS;
}