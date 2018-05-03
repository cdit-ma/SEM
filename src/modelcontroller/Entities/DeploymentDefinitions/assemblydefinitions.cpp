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

    setAcceptsNodeKind(NODE_KIND::COMPONENT_ASSEMBLY);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_PROFILE);
}

VIEW_ASPECT AssemblyDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::ASSEMBLIES;
}
