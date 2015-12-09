#include "deploymentdefinitions.h"
#include "assemblydefinitions.h"
#include "hardwaredefinitions.h"

DeploymentDefinitions::DeploymentDefinitions():Node()
{
}

DeploymentDefinitions::~DeploymentDefinitions()
{

}

bool DeploymentDefinitions::canAdoptChild(Node *child)
{
    AssemblyDefinitions* assemblyDefinitions = dynamic_cast<AssemblyDefinitions *>(child);
    HardwareDefinitions* hardwareDefinitions = dynamic_cast<HardwareDefinitions *>(child);

    if(!(assemblyDefinitions || hardwareDefinitions)){
        //DeploymentDefinitions can only adopt 1 HardwareDefinition/AssemblyDefinition
        return false;
    }

    foreach(Node* child, getChildren(0)){
        if(child->compareData(child, "kind")){
            //Deployment Definitions can only adopt 1 of each HardwareDefinition/AssemblyDefinition
            return false;
        }
    }

    return Node::canAdoptChild(child);
}
