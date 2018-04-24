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
    addInstancesDefinitionKind(NODE_KIND::SERVER_INTERFACE);
    addInstanceKind(NODE_KIND::CLIENT_PORT_INSTANCE);
    addImplKind(NODE_KIND::SERVER_REQUEST);
}


bool MEDEA::ClientPort::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:
    case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:{
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