#include "deploymentdefinitions.h"
#include "assemblydefinitions.h"
#include "hardwaredefinitions.h"

DeploymentDefinitions::DeploymentDefinitions():Node(NT_NODE, NC_DEPLOYMENT_DEFINITIONS)
{
}

DeploymentDefinitions::~DeploymentDefinitions()
{

}

bool DeploymentDefinitions::canAdoptChild(Node *node)
{
    AssemblyDefinitions* assemblyDefinitions = dynamic_cast<AssemblyDefinitions *>(node);
    HardwareDefinitions* hardwareDefinitions = dynamic_cast<HardwareDefinitions *>(node);

    if(!(assemblyDefinitions || hardwareDefinitions)){
        //DeploymentDefinitions can only adopt 1 HardwareDefinition/AssemblyDefinition
        return false;
    }

    foreach(Node* child, getChildren(0)){
        if(node->compareData(child, "kind")){
            //Deployment Definitions can only adopt 1 of each HardwareDefinition/AssemblyDefinition
            return false;
        }
    }

    return Node::canAdoptChild(node);
}
