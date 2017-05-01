#include "blackbox.h"
#include "../entityfactory.h"

BlackBox::BlackBox(EntityFactory* factory) : Node(factory, NODE_KIND::BLACKBOX, "BlackBox"){
	auto node_kind = NODE_KIND::BLACKBOX;
	QString kind_string = "BlackBox";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new BlackBox();});
};

BlackBox::BlackBox():Node(NODE_KIND::BLACKBOX)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);

    setInstanceKind(NODE_KIND::BLACKBOX_INSTANCE);
}

bool BlackBox::canAdoptChild(Node *child)
{
    //Can Only adopt EventPort Definitions
    switch(child->getNodeKind()){
    case NODE_KIND::INEVENTPORT:
    case NODE_KIND::OUTEVENTPORT:
        break;
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool BlackBox::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
