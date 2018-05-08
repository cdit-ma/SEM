#include "deploymentdefinitions.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::DEPLOYMENT_DEFINITIONS;
const QString kind_string = "DeploymentDefinitions";

void DeploymentDefinitions::RegisterWithEntityFactory(EntityFactory* factory){
	RegisterNodeKind(factory, node_kind, kind_string, 0);
    RegisterComplexNodeKind(factory, node_kind, [](EntityFactory* ef, bool is_temp){return new DeploymentDefinitions(ef, is_temp);});
}


DeploymentDefinitions::DeploymentDefinitions(EntityFactory* factory, bool is_temp) : Node(node_kind)
{
    if(is_temp){
        return;
    }
    setAcceptsNodeKind(NODE_KIND::ASSEMBLY_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_DEFINITIONS);
    auto assembly = factory->CreateNode(NODE_KIND::ASSEMBLY_DEFINITIONS);
    auto hardware = factory->CreateNode(NODE_KIND::HARDWARE_DEFINITIONS);

    if(assembly && hardware){
        auto adopt_success = addChild(assembly);
        adopt_success = adopt_success && addChild(hardware);

        if(adopt_success){
            return;
        }
    }

    factory->DestructEntity(assembly);
    factory->DestructEntity(hardware);
    throw std::invalid_argument("Model cannot be constructed");
}

bool DeploymentDefinitions::canAdoptChild(Node *node)
{
    auto node_kind = node->getNodeKind();
    if(getChildrenOfKindCount(node_kind) > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}