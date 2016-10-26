#include "deploymentdefinitions.h"

DeploymentDefinitions::DeploymentDefinitions():Node(NK_DEPLOYMENT_DEFINITIONS)
{
}

bool DeploymentDefinitions::canAdoptChild(Node *node)
{
    if(!node->isNodeOfType(NT_ASPECT)){
        return false;
    }
    switch(node->getNodeKind()){
    case NK_ASSEMBLY_DEFINITIONS:
    case NK_HARDWARE_DEFINITIONS:
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
