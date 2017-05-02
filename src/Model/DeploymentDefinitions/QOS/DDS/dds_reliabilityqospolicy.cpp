#include "dds_reliabilityqospolicy.h"
#include "../../../entityfactory.h"
DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_RELIABILITY, "DDS_ReliabilityQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_RELIABILITY;
	QString kind_string = "DDS_ReliabilityQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_ReliabilityQosPolicy();});
};
DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_RELIABILITY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);


    //setMoveEnabled(false);
    //setExpandEnabled(false);
    //updateDefaultData("label", QVariant::String, true, "reliability");
    //updateDefaultData("qos_dds_kind", QVariant::String, false, "RELIABLE_RELIABILITY_QOS");
    //updateDefaultData("qos_dds_max_blocking_time_sec", QVariant::String, false, "0");
    //updateDefaultData("qos_dds_max_blocking_time_nanosec", QVariant::String, false, "100000000");

    //qos_dds_kind
    auto values = QStringList();
    values << "BEST_EFFORT_RELIABILITY_QOS";
    values << "RELIABLE_RELIABILITY_QOS";
}

bool DDS_ReliabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ReliabilityQosPolicy::canAcceptEdge(EDGE_KIND, Node *){
    return false;
}