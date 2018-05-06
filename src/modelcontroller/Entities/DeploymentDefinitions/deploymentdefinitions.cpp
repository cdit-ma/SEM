#include "deploymentdefinitions.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::DEPLOYMENT_DEFINITIONS;
const QString kind_string = "DeploymentDefinitions";

DeploymentDefinitions::DeploymentDefinitions(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DeploymentDefinitions();});
    RegisterComplexNodeKind(factory, node_kind, &DeploymentDefinitions::ConstructDeploymentDefinitions);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "DEPLOYMENT");
};

DeploymentDefinitions::DeploymentDefinitions() : Node(node_kind)
{
    setAcceptsNodeKind(NODE_KIND::ASSEMBLY_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_DEFINITIONS);
}

bool DeploymentDefinitions::canAdoptChild(Node *node)
{
    auto node_kind = node->getNodeKind();
    if(getChildrenOfKindCount(node_kind) > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}
#include <QDebug>

Node* DeploymentDefinitions::ConstructDeploymentDefinitions(EntityFactory* factory){
    //Don't recurse into the complex function
    auto deployment = factory->CreateNode(NODE_KIND::DEPLOYMENT_DEFINITIONS, false);
    auto assembly = factory->CreateNode(NODE_KIND::ASSEMBLY_DEFINITIONS);
    auto hardware = factory->CreateNode(NODE_KIND::HARDWARE_DEFINITIONS);

    if(deployment && assembly && hardware){
        auto adopt_success = deployment->addChild(assembly);
        adopt_success = adopt_success && deployment->addChild(hardware);

        if(adopt_success){
            return deployment;
        }
    }

    factory->DestructEntity(deployment);
    factory->DestructEntity(assembly);
    factory->DestructEntity(hardware);
    return 0;
}