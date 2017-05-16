#include "component.h"


Component::Component(EntityFactory* factory) : Node(factory, NODE_KIND::COMPONENT, "Component"){
	auto node_kind = NODE_KIND::COMPONENT;
	QString kind_string = "Component";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Component();});
};

Component::Component():Node(NODE_KIND::COMPONENT)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);

    setInstanceKind(NODE_KIND::COMPONENT_INSTANCE);
    setImplKind(NODE_KIND::COMPONENT_IMPL);
}

bool Component::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::ATTRIBUTE:
    case NODE_KIND::INEVENTPORT:
    case NODE_KIND::OUTEVENTPORT:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Component::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
