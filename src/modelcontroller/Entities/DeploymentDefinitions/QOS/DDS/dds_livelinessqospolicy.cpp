#include "dds_livelinessqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_LIVELINESS;
const static QString kind_string = "DDS_LivelinessQosPolicy";

void DDS_LivelinessQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_LivelinessQosPolicy(factory, is_temp_node);
    });
}

DDS_LivelinessQosPolicy::DDS_LivelinessQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    QList<QVariant> values;
    values << "AUTOMATIC_LIVELINESS_QOS";
    values << "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS";
    values << "MANUAL_BY_TOPIC_LIVELINESS_QOS";
    factory.AttachData(this, "label", QVariant::String, "liveliness", true);
    factory.AttachData(this, "qos_dds_lease_duration_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    factory.AttachData(this, "qos_dds_lease_duration_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
    auto dds_kind_data = factory.AttachData(this, "qos_dds_kind", QVariant::String, values.first(), false);
    dds_kind_data->addValidValues(values);
}