#include "hardwaredefinitions.h"
#include "../entityfactory.h"

HardwareDefinitions::HardwareDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::HARDWARE_DEFINITIONS, "HardwareDefinitions"){
	auto node_kind = NODE_KIND::HARDWARE_DEFINITIONS;
	QString kind_string = "HardwareDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new HardwareDefinitions();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "HARDWARE");
};

HardwareDefinitions::HardwareDefinitions():Node(NODE_KIND::HARDWARE_DEFINITIONS)
{
    setNodeType(NODE_TYPE::ASPECT);
    setMoveEnabled(false);
    setExpandEnabled(false);
}

VIEW_ASPECT HardwareDefinitions::getViewAspect() const
{
    return VA_HARDWARE;
}

bool HardwareDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::HARDWARE_CLUSTER:
    case NODE_KIND::LOGGINGPROFILE:
    case NODE_KIND::LOGGINGSERVER:
    case NODE_KIND::HARDWARE_NODE:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool HardwareDefinitions::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
