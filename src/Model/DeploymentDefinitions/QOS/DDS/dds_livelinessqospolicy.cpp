#include "dds_livelinessqospolicy.h"
#include "../../../data.h"

#include "../../../entityfactory.h"
DDS_LivelinessQosPolicy::DDS_LivelinessQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_LIVELINESS, "DDS_LivelinessQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_LIVELINESS;
	QString kind_string = "DDS_LivelinessQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_LivelinessQosPolicy();});
};
DDS_LivelinessQosPolicy::DDS_LivelinessQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_LIVELINESS)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "liveliness");
    updateDefaultData("qos_dds_kind", QVariant::String, false, "AUTOMATIC_LIVELINESS_QOS");
 
    //qos_dds_kind
    auto values = QStringList();
    values << "AUTOMATIC_LIVELINESS_QOS";
    values << "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS";
    values << "MANUAL_BY_TOPIC_LIVELINESS_QOS";

    updateDefaultData("qos_dds_lease_duration_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    updateDefaultData("qos_dds_lease_duration_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
}

bool DDS_LivelinessQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LivelinessQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
