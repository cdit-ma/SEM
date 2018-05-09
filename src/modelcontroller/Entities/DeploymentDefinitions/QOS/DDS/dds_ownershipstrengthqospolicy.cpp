#include "dds_ownershipstrengthqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_OWNERSHIPSTRENGTH;
const static QString kind_string = "DDS_OwnershipStrengthQosPolicy";

void DDS_OwnershipStrengthQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_OwnershipStrengthQosPolicy(factory, is_temp_node);
    });
}

DDS_OwnershipStrengthQosPolicy::DDS_OwnershipStrengthQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "ownership_strength", true);
    factory.AttachData(this, "qos_dds_int_value", QVariant::Int, 0, false);
}