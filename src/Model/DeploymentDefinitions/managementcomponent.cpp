#include "managementcomponent.h"
#include "../entityfactory.h"

ManagementComponent::ManagementComponent(EntityFactory* factory) : Node(factory, NODE_KIND::MANAGEMENT_COMPONENT, "ManagementComponent"){
	auto node_kind = NODE_KIND::MANAGEMENT_COMPONENT;
	QString kind_string = "ManagementComponent";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ManagementComponent();});
};

ManagementComponent::ManagementComponent():Node(NODE_KIND::MANAGEMENT_COMPONENT)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
}

bool ManagementComponent::canAdoptChild(Node*)
{
    return false;
}

bool ManagementComponent::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
