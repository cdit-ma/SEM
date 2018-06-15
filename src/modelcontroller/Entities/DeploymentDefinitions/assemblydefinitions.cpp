#include "assemblydefinitions.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::ASSEMBLY_DEFINITIONS;
const QString kind_string = "Assembly Definitions";

void AssemblyDefinitions::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new AssemblyDefinitions(broker, is_temp_node);
        });
}

AssemblyDefinitions::AssemblyDefinitions(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_ASSEMBLY);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_PROFILE);
    setAcceptsNodeKind(NODE_KIND::EXTERNAL_PUBSUB_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::EXTERNAL_SERVER_DELEGATE);
    

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    setLabelFunctional(false);
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "ASSEMBLIES");
}

VIEW_ASPECT AssemblyDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::ASSEMBLIES;
}
