#include "serverinterface.h"

const NODE_KIND node_kind = NODE_KIND::SERVER_INTERFACE;
const QString kind_string = "ServerInterface";

MEDEA::ServerInterface::ServerInterface(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ServerInterface();});
};

MEDEA::ServerInterface::ServerInterface(): Node(node_kind)
{
    setNodeType(NODE_TYPE::DEFINITION);
    
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
}

bool MEDEA::ServerInterface::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::INPUT_PARAMETER_GROUP:
    case NODE_KIND::RETURN_PARAMETER_GROUP:{
        if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
            return false;
        }
        break;
    }
    default:
        return false;
    };
    return Node::canAdoptChild(child);
}

bool MEDEA::ServerInterface::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    return Node::canAcceptEdge(edge_kind, dst);
}
