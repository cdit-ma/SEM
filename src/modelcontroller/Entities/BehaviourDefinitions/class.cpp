#include "class.h"

const NODE_KIND node_kind = NODE_KIND::CLASS;
const QString kind_string = "Class";

MEDEA::Class::Class(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Class();});
    
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, false);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, false);
};

MEDEA::Class::Class(): Node(node_kind)
{
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    addInstanceKind(NODE_KIND::CLASS_INSTANCE);
}

bool MEDEA::Class::Class::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::ATTRIBUTE:
        case NODE_KIND::FUNCTION:
        case NODE_KIND::EXTERNAL_TYPE:
        case NODE_KIND::CLASS_INSTANCE:
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
