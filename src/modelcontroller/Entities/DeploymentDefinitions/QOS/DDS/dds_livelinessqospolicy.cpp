#include "dds_livelinessqospolicy.h"

DDS_LivelinessQosPolicy::DDS_LivelinessQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_LIVELINESS, "DDS_LivelinessQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_LIVELINESS;
	QString kind_string = "DDS_LivelinessQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_LivelinessQosPolicy();});

    QList<QVariant> values;
    values << "AUTOMATIC_LIVELINESS_QOS";
    values << "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS";
    values << "MANUAL_BY_TOPIC_LIVELINESS_QOS";

    RegisterValidDataValues(factory, node_kind, "qos_dds_kind", QVariant::String, values);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "liveliness");
    RegisterDefaultData(factory, node_kind, "qos_dds_kind", QVariant::String, false, values.first());
    RegisterDefaultData(factory, node_kind, "qos_dds_lease_duration_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    RegisterDefaultData(factory, node_kind, "qos_dds_lease_duration_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
};

DDS_LivelinessQosPolicy::DDS_LivelinessQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_LIVELINESS)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}