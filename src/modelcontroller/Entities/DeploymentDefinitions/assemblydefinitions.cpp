#include "assemblydefinitions.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::ASSEMBLY_DEFINITIONS;
const QString kind_string = "AssemblyDefinitions";

void AssemblyDefinitions::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new AssemblyDefinitions(factory, is_temp_node);
        });
}

AssemblyDefinitions::AssemblyDefinitions(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_ASSEMBLY);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_PROFILE);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "ASSEMBLIES", true);
}

VIEW_ASPECT AssemblyDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::ASSEMBLIES;
}
