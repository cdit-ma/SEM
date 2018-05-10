#include "dds_partitionqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_PARTITION;
const static QString kind_string = "DDS_PartitionQosPolicy";

void DDS_PartitionQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_PartitionQosPolicy(broker, is_temp_node);
    });
}

DDS_PartitionQosPolicy::DDS_PartitionQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);





    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "partition", true);
    broker.AttachData(this, "qos_dds_name", QVariant::String, "", false);
}