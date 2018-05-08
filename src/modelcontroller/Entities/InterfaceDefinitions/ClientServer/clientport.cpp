#include "clientport.h"
#include "../../../entityfactory.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::CLIENT_PORT;
const QString kind_string = "ClientPort";


void MEDEA::ClientPort::RegisterWithEntityFactory(EntityFactory& factory){
	Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ClientPort(factory, is_temp_node);
        });
};

MEDEA::ClientPort::ClientPort(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::SERVER_INTERFACE);
    addInstanceKind(NODE_KIND::CLIENT_PORT_INSTANCE);
    addImplKind(NODE_KIND::SERVER_REQUEST);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE);

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "index", QVariant::String, -1, false);
}


bool MEDEA::ClientPort::canAdoptChild(Node* child)
{
    auto child_node_kind = child->getNodeKind();

    switch(child_node_kind){
    case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:
    case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:{
        if(getChildrenOfKindCount(child_node_kind) > 0){
            return false;
        }
        break;
    }
    default:
        break;
    };
    return Node::canAdoptChild(child);
}