#include "dds_deadlineqospolicy.h"

#include "../../../entityfactory.h"
DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_DEADLINE, "DDS_DeadlineQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_DEADLINE;
	QString kind_string = "DDS_DeadlineQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_DeadlineQosPolicy();});
};
DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_DEADLINE)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "deadline");
    updateDefaultData("qos_dds_period_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    updateDefaultData("qos_dds_period_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
}

bool DDS_DeadlineQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DeadlineQosPolicy::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    Q_UNUSED(edgeKind);
    Q_UNUSED(dst);
    return false;
}
