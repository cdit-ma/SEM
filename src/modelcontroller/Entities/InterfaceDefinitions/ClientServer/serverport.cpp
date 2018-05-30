#include "serverport.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::SERVER_PORT;
const QString kind_string = "Server Port";

void MEDEA::ServerPort::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
	broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ServerPort(broker, is_temp_node);
        });
};

MEDEA::ServerPort::ServerPort(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::SERVER_INTERFACE);
    addInstanceKind(NODE_KIND::SERVER_PORT_INSTANCE);
    addImplKind(NODE_KIND::SERVER_PORT_IMPL);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "index", QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "row", QVariant::Int, ProtectedState::PROTECTED, 0);
}

bool MEDEA::ServerPort::canAdoptChild(Node* child)
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


