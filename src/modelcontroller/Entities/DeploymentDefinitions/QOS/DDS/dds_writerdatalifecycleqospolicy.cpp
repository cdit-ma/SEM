#include "dds_writerdatalifecycleqospolicy.h"

DDS_WriterDataLifecycleQosPolicy::DDS_WriterDataLifecycleQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE, "DDS_WriterDataLifecycleQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE;
	QString kind_string = "DDS_WriterDataLifecycleQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_WriterDataLifecycleQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "writer_data_lifecycle");
    RegisterDefaultData(factory, node_kind, "qos_dds_autodispose_unregistered_instances", QVariant::Bool, false, true);
}

DDS_WriterDataLifecycleQosPolicy::DDS_WriterDataLifecycleQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}