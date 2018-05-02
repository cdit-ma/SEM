#include "hardwaredefinitions.h"

#include "../../edgekinds.h"

HardwareDefinitions::HardwareDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::HARDWARE_DEFINITIONS, "HardwareDefinitions"){
	auto node_kind = NODE_KIND::HARDWARE_DEFINITIONS;
	QString kind_string = "HardwareDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new HardwareDefinitions();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "HARDWARE");
};

HardwareDefinitions::HardwareDefinitions():Node(NODE_KIND::HARDWARE_DEFINITIONS)
{
    setNodeType(NODE_TYPE::ASPECT);

    setAcceptsNodeKind(NODE_KIND::HARDWARE_CLUSTER);
    setAcceptsNodeKind(NODE_KIND::LOGGINGPROFILE);
    setAcceptsNodeKind(NODE_KIND::LOGGINGSERVER);
    setAcceptsNodeKind(NODE_KIND::HARDWARE_NODE);
}

VIEW_ASPECT HardwareDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::HARDWARE;
}
