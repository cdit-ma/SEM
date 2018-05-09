#include "dds_partitionqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_PARTITION;
const static QString kind_string = "DDS_PartitionQosPolicy";

void DDS_PartitionQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_PartitionQosPolicy(factory, is_temp_node);
    });
}

DDS_PartitionQosPolicy::DDS_PartitionQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "partition", true);
    factory.AttachData(this, "qos_dds_name", QVariant::String, "", false);
}