#include "deploymentdefinitions.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::DEPLOYMENT_DEFINITIONS;
const QString kind_string = "Deployment Definitions";

void DeploymentDefinitions::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DeploymentDefinitions(broker, is_temp_node);
    });
}

DeploymentDefinitions::DeploymentDefinitions(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setAcceptsNodeKind(NODE_KIND::ASSEMBLY_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_DEFINITIONS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "DEPLOYMENT");

    //Attach Children
    broker.ConstructChildNode(*this, NODE_KIND::ASSEMBLY_DEFINITIONS);
    broker.ConstructChildNode(*this, NODE_KIND::HARDWARE_DEFINITIONS);
}

bool DeploymentDefinitions::canAdoptChild(Node *node)
{
    auto node_kind = node->getNodeKind();
    if(getChildrenOfKindCount(node_kind) > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}