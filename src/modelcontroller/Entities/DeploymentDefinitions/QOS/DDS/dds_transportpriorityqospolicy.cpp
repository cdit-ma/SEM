#include "dds_transportpriorityqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_TRANSPORTPRIORITY;
const static QString kind_string = "DDS_TransportPriorityQosPolicy";

void DDS_TransportPriorityQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_TransportPriorityQosPolicy(broker, is_temp_node);
    });
}

DDS_TransportPriorityQosPolicy::DDS_TransportPriorityQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);




    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "transport_priority", true);
    broker.AttachData(this, "qos_dds_int_value", QVariant::Int, 0, false);
}