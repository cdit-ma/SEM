#include "assemblydefinitions.h"
#include "componentassembly.h"
#include "managementcomponent.h"
#include "QOS/qosprofile.h"

AssemblyDefinitions::AssemblyDefinitions():Node(Node::NT_ASPECT)
{
}

AssemblyDefinitions::~AssemblyDefinitions()
{

}

VIEW_ASPECT AssemblyDefinitions::getViewAspect()
{
    return VA_ASSEMBLIES;
}

bool AssemblyDefinitions::canAdoptChild(Node *node)
{
    ComponentAssembly* componentAssembly = dynamic_cast<ComponentAssembly *>(node);
    ManagementComponent* managementComponent = dynamic_cast<ManagementComponent *>(node);
    QOSProfile* qosProfile = dynamic_cast<QOSProfile *>(node);

    if(!(componentAssembly || managementComponent || qosProfile)){
        //AssemblyDefinition can only adopt ComponentAssemblies or ManagementComponents
        return false;
    }

    if(managementComponent){
        foreach(Node* child, getChildrenOfKind("ManagementComponent", 0)){
            if(node->compareData(child, "type")){
                //AssemblyDefinition can only adopt ManagementComponents with different types.
                return false;
            }
        }
    }
    return Node::canAdoptChild(node);
}
