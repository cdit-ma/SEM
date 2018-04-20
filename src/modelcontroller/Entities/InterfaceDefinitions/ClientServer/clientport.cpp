#include "clientport.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::CLIENT_PORT;
const QString kind_string = "ClientPort";

MEDEA::ClientPort::ClientPort(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ClientPort();});
};

MEDEA::ClientPort::ClientPort(): Node(node_kind)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);

    setDefinitionKind(NODE_KIND::SERVER_INTERFACE);
    
    setImplKind(NODE_KIND::SERVER_REQUEST);
    setInstanceKind(NODE_KIND::CLIENT_PORT_INSTANCE);
}


bool MEDEA::ClientPort::canAdoptChild(Node* child)
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

bool MEDEA::ClientPort::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(!acceptsEdgeKind(edge_kind)){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != NODE_KIND::SERVER_INTERFACE){//} || dst->getNodeKind() == NODE_KIND::SERVER_REQUEST)){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}
