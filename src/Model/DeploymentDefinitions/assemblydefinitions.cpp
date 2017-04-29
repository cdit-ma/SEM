#include "assemblydefinitions.h"

AssemblyDefinitions::AssemblyDefinitions():Node(Node::NK_ASSEMBLY_DEFINITIONS)
{
    setNodeType(NT_ASPECT);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "ASSEMBLIES");
}

VIEW_ASPECT AssemblyDefinitions::getViewAspect() const
{
    return VA_ASSEMBLIES;
}

bool AssemblyDefinitions::canAdoptChild(Node *node)
{
    switch(node->getNodeKind()){
    case NK_COMPONENT_ASSEMBLY:

    case NK_MANAGEMENT_COMPONENT:
        foreach(Node* child, getChildrenOfKind(NK_MANAGEMENT_COMPONENT, 0)){
            if(node->compareData(child, "type")){
                //AssemblyDefinition can only adopt ManagementComponents with different types.
                return false;
            }
        }
        break;
    default:
        //Check for QOS profiles.
        if(!node->isNodeOfType(NT_QOS_PROFILE)){
            return false;
        }
    }
    return Node::canAdoptChild(node);
}

bool AssemblyDefinitions::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
