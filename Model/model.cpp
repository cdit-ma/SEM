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

bool Model::canAdoptChild(Node *child)
{
    InterfaceDefinitions* interfaceDefinitions = dynamic_cast<InterfaceDefinitions*>(child);
    DeploymentDefinitions* deploymentDefinitions = dynamic_cast<DeploymentDefinitions*>(child);
    BehaviourDefinitions* behaviourDefinitions = dynamic_cast<BehaviourDefinitions*>(child);

    if(!(behaviourDefinitions || deploymentDefinitions || interfaceDefinitions)){
        return false;
    }

    foreach(Node* child, getChildren(0)){
        if(child->compareData(child, "kind")){
            //Model can only adopt 1 of each adoptable Definitions
            return false;
        }
    }

    return Node::canAdoptChild(child);
}
