#include "deploymentattribute.h"
#include "../../entityfactory.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::DEPLOYMENT_ATTRIBUTE;
const QString kind_string = "DeploymentAttribute";

void MEDEA::DeploymentAttribute::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::DeploymentAttribute(factory, is_temp_node);
        });
}


MEDEA::DeploymentAttribute::DeploymentAttribute(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    setDataProducer(true);
    setDataReceiver(true);
    setMultipleDataProducer(true);

    //Setup Data
    auto type_data = factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "comment", QVariant::String, "", false);
    type_data->addValidValues(TypeKey::GetValidPrimitiveTypes());
}

bool MEDEA::DeploymentAttribute::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    QSet<NODE_KIND> valid_data_kinds = {NODE_KIND::ATTRIBUTE_INSTANCE, NODE_KIND::DEPLOYMENT_ATTRIBUTE};

    switch(edge_kind){
    case EDGE_KIND::DATA:{
        if(!valid_data_kinds.contains(dst->getNodeKind())){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return DataNode::canAcceptEdge(edge_kind, dst);
}