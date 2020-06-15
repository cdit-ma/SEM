#include "deploymentattribute.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::DEPLOYMENT_ATTRIBUTE;
const QString kind_string = "Deployment Attribute";

void MEDEA::DeploymentAttribute::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::DeploymentAttribute(broker, is_temp_node);
        });
}


MEDEA::DeploymentAttribute::DeploymentAttribute(::EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp, false){
    //Setup State
    setDataProducer(true);
    setDataReceiver(true);
    setMultipleDataProducer(true);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    auto type_data = broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::UNPROTECTED, TypeKey::GetDefaultPrimitiveType());
    type_data->addValidValues(TypeKey::GetValidPrimitiveTypes());

    broker.AttachData(this, KeyName::Comment, QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED);
}

bool MEDEA::DeploymentAttribute::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }
    
    const static QSet<NODE_KIND> valid_dst_kinds = {NODE_KIND::ATTRIBUTE_INST, NODE_KIND::DEPLOYMENT_ATTRIBUTE, NODE_KIND::VARIABLE};

    switch(edge_kind){
    case EDGE_KIND::DATA:{
        if(!valid_dst_kinds.contains(dst->getNodeKind()) || dst->getViewAspect() != VIEW_ASPECT::ASSEMBLIES){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return DataNode::canAcceptEdge(edge_kind, dst);
}