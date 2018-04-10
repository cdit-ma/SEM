#include "assemblydefinitions.h"


#include "../../edgekinds.h"


AssemblyDefinitions::AssemblyDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::ASSEMBLY_DEFINITIONS, "AssemblyDefinitions"){
	auto node_kind = NODE_KIND::ASSEMBLY_DEFINITIONS;
	QString kind_string = "AssemblyDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new AssemblyDefinitions();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "ASSEMBLIES");
};

AssemblyDefinitions::AssemblyDefinitions():Node(NODE_KIND::ASSEMBLY_DEFINITIONS)
{
    setNodeType(NODE_TYPE::ASPECT);
    //setMoveEnabled(false);
    //setExpandEnabled(false);
}

VIEW_ASPECT AssemblyDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::ASSEMBLIES;
}

bool AssemblyDefinitions::canAdoptChild(Node *node)
{
    switch(node->getNodeKind()){
    case NODE_KIND::COMPONENT_ASSEMBLY:
    case NODE_KIND::COMPONENT_INSTANCE:
    case NODE_KIND::DEPLOYMENT_ATTRIBUTE:
        break;
    case NODE_KIND::MANAGEMENT_COMPONENT:
        foreach(Node* child, getChildrenOfKind(NODE_KIND::MANAGEMENT_COMPONENT, 0)){
            if(node->compareData(child, "type")){
                //AssemblyDefinition can only adopt ManagementComponents with different types.
                return false;
            }
        }
        break;
    default:
        //Check for QOS profiles.
        if(!node->isNodeOfType(NODE_TYPE::QOS_PROFILE)){
            return false;
        }
    }
    return Node::canAdoptChild(node);
}

bool AssemblyDefinitions::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
