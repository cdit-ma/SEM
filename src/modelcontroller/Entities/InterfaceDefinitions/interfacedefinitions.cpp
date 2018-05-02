#include "interfacedefinitions.h"


InterfaceDefinitions::InterfaceDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::INTERFACE_DEFINITIONS, "InterfaceDefinitions"){
	auto node_kind = NODE_KIND::INTERFACE_DEFINITIONS;
	QString kind_string = "InterfaceDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InterfaceDefinitions();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "INTERFACES");
};

InterfaceDefinitions::InterfaceDefinitions(): Node(NODE_KIND::INTERFACE_DEFINITIONS)
{
    setNodeType(NODE_TYPE::ASPECT);

    setAcceptsNodeKind(NODE_KIND::ENUM);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE);
    setAcceptsNodeKind(NODE_KIND::COMPONENT);
    setAcceptsNodeKind(NODE_KIND::SHARED_DATATYPES);
    setAcceptsNodeKind(NODE_KIND::NAMESPACE);
    setAcceptsNodeKind(NODE_KIND::SERVER_INTERFACE);
}

VIEW_ASPECT InterfaceDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::INTERFACES;
}