#include "dds_reliabilityqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_RELIABILITY;
const static QString kind_string = "DDS_ReliabilityQosPolicy";

void DDS_ReliabilityQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_ReliabilityQosPolicy(factory, is_temp_node);
    });
}

DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    QList<QVariant> values;
    values << "BEST_EFFORT_RELIABILITY_QOS";
    values << "RELIABLE_RELIABILITY_QOS";
    factory.AttachData(this, "label", QVariant::String, "reliability", true);
    factory.AttachData(this, "qos_dds_max_blocking_time_sec", QVariant::String, "0", false);
    factory.AttachData(this, "qos_dds_max_blocking_time_nanosec", QVariant::String, "100000000", false);
    auto dds_kinds_data = factory.AttachData(this, "qos_dds_kind", QVariant::String, "0", false);
    dds_kinds_data->addValidValues(values);
}