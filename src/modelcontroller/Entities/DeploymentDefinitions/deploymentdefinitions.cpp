#include "deploymentdefinitions.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::DEPLOYMENT_DEFINITIONS;
const QString kind_string = "DeploymentDefinitions";

void DeploymentDefinitions::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DeploymentDefinitions(factory, is_temp_node);
    });
}

DeploymentDefinitions::DeploymentDefinitions(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setAcceptsNodeKind(NODE_KIND::ASSEMBLY_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_DEFINITIONS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "DEPLOYMENT", true);

    //Attach Children
    factory.ConstructChildNode(*this, NODE_KIND::ASSEMBLY_DEFINITIONS);
    factory.ConstructChildNode(*this, NODE_KIND::HARDWARE_DEFINITIONS);
}

bool DeploymentDefinitions::canAdoptChild(Node *node)
{
    auto node_kind = node->getNodeKind();
    if(getChildrenOfKindCount(node_kind) > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}