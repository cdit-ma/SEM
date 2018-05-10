#include "dds_destinationorderqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_DESTINATIONORDER;
const static QString kind_string = "DDS_DestinationOrderQosPolicy";

void DDS_DestinationOrderQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_DestinationOrderQosPolicy(broker, is_temp_node);
    });
}

DDS_DestinationOrderQosPolicy::DDS_DestinationOrderQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    QList<QVariant> values({"BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS", "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS"});
    broker.AttachData(this, "label", QVariant::String, "destination_order", true);
    auto dds_kind_data = broker.AttachData(this, "qos_dds_kind", QVariant::String, values.first(), false);
    dds_kind_data->addValidValues(values);
}