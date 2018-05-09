#include "dds_durabilityqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_DURABILITY;
const static QString kind_string = "DDS_DurabilityQosPolicy";

void DDS_DurabilityQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_DurabilityQosPolicy(factory, is_temp_node);
    });
}

DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    QList<QVariant> values;
    values << "VOLATILE_DURABILITY_QOS";
    values << "TRANSIENT_LOCAL_DURABILITY_QOS";
    values << "TRANSIENT_DURABILITY_QOS";
    values << "PERSISTENT_DURABILITY_QOS";
    factory.AttachData(this, "label", QVariant::String, "durability", true);
    auto dds_kind_data = factory.AttachData(this, "qos_dds_kind", QVariant::String, values.first(), true);
    dds_kind_data->addValidValues(values);
}