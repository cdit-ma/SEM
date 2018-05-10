#include "dds_ownershipqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_OWNERSHIP;
const static QString kind_string = "DDS_OwnershipQosPolicy";

void DDS_OwnershipQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_OwnershipQosPolicy(broker, is_temp_node);
    });
}

DDS_OwnershipQosPolicy::DDS_OwnershipQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);




    //Setup Data
    QList<QVariant> values;
    values << "SHARED_OWNERSHIP_QOS";
    values << "EXCLUSIVE_OWNERSHIP_QOS";
    broker.AttachData(this, "label", QVariant::String, "ownership", true);
    auto dds_kind_data = broker.AttachData(this, "qos_dds_kind", QVariant::String, values.first(), false);
    dds_kind_data->addValidValues(values);
}