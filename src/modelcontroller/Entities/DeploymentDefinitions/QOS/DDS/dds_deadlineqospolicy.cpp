#include "dds_deadlineqospolicy.h"
DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_DEADLINE, "DDS_DeadlineQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_DEADLINE;
	QString kind_string = "DDS_DeadlineQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_DeadlineQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "deadline");
    RegisterDefaultData(factory, node_kind, "qos_dds_period_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    RegisterDefaultData(factory, node_kind, "qos_dds_period_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
};

DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_DEADLINE)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}