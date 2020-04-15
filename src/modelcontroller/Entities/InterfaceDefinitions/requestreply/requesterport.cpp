#include "requesterport.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::PORT_REQUESTER;
const QString kind_string = "Requester Port";


void MEDEA::RequesterPort::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
	broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::RequesterPort(broker, is_temp_node);
        });
};

MEDEA::RequesterPort::RequesterPort(::EntityFactoryBroker& broker, bool is_temp) : EventPort(broker, node_kind, is_temp, false){
    //Setup State
    addInstanceKind(NODE_KIND::PORT_REQUESTER_INST);
    addImplKind(NODE_KIND::PORT_REQUESTER_IMPL);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INST);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INST);
    
    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Index, QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 2);
}


bool MEDEA::RequesterPort::canAdoptChild(Node* child)
{
    auto child_node_kind = child->getNodeKind();

    switch(child_node_kind){
    case NODE_KIND::INPUT_PARAMETER_GROUP_INST:
    case NODE_KIND::RETURN_PARAMETER_GROUP_INST:{
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