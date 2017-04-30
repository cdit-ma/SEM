#include "dds_writerdatalifecycleqospolicy.h"

DDS_WriterDataLifecycleQosPolicy::DDS_WriterDataLifecycleQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "writer_data_lifecycle");
    updateDefaultData("qos_dds_autodispose_unregistered_instances", QVariant::Bool, false, true);
}

bool DDS_WriterDataLifecycleQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_WriterDataLifecycleQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
