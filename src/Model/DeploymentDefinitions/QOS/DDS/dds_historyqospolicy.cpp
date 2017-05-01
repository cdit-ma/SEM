#include "dds_historyqospolicy.h"

#include "../../../entityfactory.h"
DDS_HistoryQosPolicy::DDS_HistoryQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_HISTORY, "DDS_HistoryQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_HISTORY;
	QString kind_string = "DDS_HistoryQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_HistoryQosPolicy();});
};
DDS_HistoryQosPolicy::DDS_HistoryQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_HISTORY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "history");
    updateDefaultData("qos_dds_kind", QVariant::String, false, "KEEP_LAST_HISTORY_QOS");
    auto values = QStringList();
    values << "KEEP_LAST_HISTORY_QOS";
    values << "KEEP_ALL_HISTORY_QOS";

    updateDefaultData("qos_dds_depth", QVariant::String, false, "1");
}

bool DDS_HistoryQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_HistoryQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}