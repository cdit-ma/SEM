#include "model.h"

Model::Model(): Node()
{
    setTop(0);
}

Model::~Model()
{
    removeEdges();
    removeChildren();
}

bool Model::canAdoptChild(Node *node)
{
    InterfaceDefinitions* interfaceDefinitions = dynamic_cast<InterfaceDefinitions*>(node);
    DeploymentDefinitions* deploymentDefinitions = dynamic_cast<DeploymentDefinitions*>(node);
    BehaviourDefinitions* behaviourDefinitions = dynamic_cast<BehaviourDefinitions*>(node);

    if(!(behaviourDefinitions || deploymentDefinitions || interfaceDefinitions)){
        return false;
    }

    foreach(Node* child, getChildren(0)){
        if(node->compareData(child, "kind")){
            //Model can only adopt 1 of each adoptable Definitions
            return false;
        }
    }

    return Node::canAdoptChild(node);
}
