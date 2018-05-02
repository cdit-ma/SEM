#include "componentassembly.h"

#include "../../edgekinds.h"

ComponentAssembly::ComponentAssembly(EntityFactory* factory) : Node(factory, NODE_KIND::COMPONENT_ASSEMBLY, "ComponentAssembly"){
	auto node_kind = NODE_KIND::COMPONENT_ASSEMBLY;
	QString kind_string = "ComponentAssembly";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ComponentAssembly();});

    RegisterDefaultData(factory, node_kind, "replicate_count", QVariant::Int, false, 1);
    RegisterDefaultData(factory, node_kind, "comment", QVariant::String);
};

ComponentAssembly::ComponentAssembly():Node(NODE_KIND::COMPONENT_ASSEMBLY)
{
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE);

    setAcceptsNodeKind(NODE_KIND::COMPONENT_ASSEMBLY);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_ATTRIBUTE);
}
