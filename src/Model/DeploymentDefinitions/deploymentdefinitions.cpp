#include "deploymentdefinitions.h"
#include "../entityfactory.h"

DeploymentDefinitions::DeploymentDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::DEPLOYMENT_DEFINITIONS, "DeploymentDefinitions"){
	auto node_kind = NODE_KIND::DEPLOYMENT_DEFINITIONS;
	QString kind_string = "DeploymentDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DeploymentDefinitions();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "DEPLOYMENT");
};

DeploymentDefinitions::DeploymentDefinitions():Node(NODE_KIND::DEPLOYMENT_DEFINITIONS)
{
    setMoveEnabled(false);
    setExpandEnabled(false);
}

bool DeploymentDefinitions::canAdoptChild(Node *node)
{
    if(!node->isNodeOfType(NODE_TYPE::ASPECT)){
        return false;
    }
    switch(node->getNodeKind()){
    case NODE_KIND::ASSEMBLY_DEFINITIONS:
    case NODE_KIND::HARDWARE_DEFINITIONS:
        foreach(Node* child, getChildren(0)){
            if(child->getNodeKind() == node->getNodeKind()){
                return false;
            }
        }
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(node);
}

bool DeploymentDefinitions::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
