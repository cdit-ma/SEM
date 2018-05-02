#include "dds_lifespanqospolicy.h"

DDS_LifespanQosPolicy::DDS_LifespanQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_LIFESPAN, "DDS_LifespanQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_LIFESPAN;
	QString kind_string = "DDS_LifespanQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_LifespanQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "lifespan");
    RegisterDefaultData(factory, node_kind, "qos_dds_duration_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    RegisterDefaultData(factory, node_kind, "qos_dds_duration_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
};

DDS_LifespanQosPolicy::DDS_LifespanQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_LIFESPAN)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}
