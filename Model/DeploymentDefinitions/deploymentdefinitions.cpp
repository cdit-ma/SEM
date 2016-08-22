#include "deploymentdefinitions.h"

DeploymentDefinitions::DeploymentDefinitions():Node(NK_DEPLOYMENT_DEFINITIONS)
{
}

bool DeploymentDefinitions::canAdoptChild(Node *node)
{
    switch(node->getNodeKind()){
    case NK_ASSEMBLY_DEFINITIONS:
    case NK_HARDWARE_DEFINITIONS:
        foreach(Node* child, getChildren(0)){
            if(child->getNodeKind() != node->getNodeKind()){
                return false;
            }
        }
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(node);
}

bool DeploymentDefinitions::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
