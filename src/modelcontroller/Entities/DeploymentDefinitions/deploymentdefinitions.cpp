#include "deploymentdefinitions.h"

#include "../../edgekinds.h"

DeploymentDefinitions::DeploymentDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::DEPLOYMENT_DEFINITIONS, "DeploymentDefinitions"){
	auto node_kind = NODE_KIND::DEPLOYMENT_DEFINITIONS;
	QString kind_string = "DeploymentDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DeploymentDefinitions();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "DEPLOYMENT");
};

DeploymentDefinitions::DeploymentDefinitions():Node(NODE_KIND::DEPLOYMENT_DEFINITIONS)
{
    setAcceptsNodeKind(NODE_KIND::ASSEMBLY_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_DEFINITIONS);
}

bool DeploymentDefinitions::canAdoptChild(Node *node)
{

    auto node_kind = node->getNodeKind();

    auto children = getChildrenOfKind(node_kind);
    if(children.size() > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}
