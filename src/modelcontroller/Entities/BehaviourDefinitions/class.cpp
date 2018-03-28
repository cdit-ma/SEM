#include "class.h"

const NODE_KIND node_kind = NODE_KIND::CLASS;
const QString kind_string = "Class";

MEDEA::Class::Class(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Class();});
};

MEDEA::Class::Class(): Node(node_kind)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    //setInstanceKind(NODE_KIND::CLASS_INSTANCE);
}

bool MEDEA::Class::Class::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::ATTRIBUTE:
        case NODE_KIND::FUNCTION:
            break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::Class::Class::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
