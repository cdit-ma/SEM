#include "idl.h"
#include "../entityfactory.h"

IDL::IDL(EntityFactory* factory) : Node(factory, NODE_KIND::IDL, "IDL"){
	auto node_kind = NODE_KIND::IDL;
	QString kind_string = "IDL";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new IDL();});
};

IDL::IDL(): Node(NODE_KIND::IDL)
{
}

bool IDL::canAdoptChild(Node *item)
{
    switch(item->getNodeKind()){
    case NODE_KIND::AGGREGATE:
    case NODE_KIND::BLACKBOX:
    case NODE_KIND::COMPONENT:
    case NODE_KIND::VECTOR:
        break;
    default:
        return false;
    }

    //Can only adopt Aggregate and Vector's in IDL's containing Aggregates/Vectors. Otherwise all children need to match kinds.
    foreach(Node* child, getChildren(0)){
        switch(child->getNodeKind()){
        case NODE_KIND::VECTOR:
        case NODE_KIND::AGGREGATE:{
            if(!(item->getNodeKind() == NODE_KIND::AGGREGATE || item->getNodeKind() == NODE_KIND::VECTOR)){
                return false;
            }
            break;
        }
        case NODE_KIND::COMPONENT:
        case NODE_KIND::BLACKBOX:
            if(child->getNodeKind() != item->getNodeKind()){
                return false;
            }
            break;
        default:
            break;
        }
    }

    return Node::canAdoptChild(item);
}

bool IDL::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
