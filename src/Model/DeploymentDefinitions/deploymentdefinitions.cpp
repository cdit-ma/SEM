#include "deploymentdefinitions.h"

DeploymentDefinitions::DeploymentDefinitions():Node(NODE_KIND::DEPLOYMENT_DEFINITIONS)
{
    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "DEPLOYMENT");
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
