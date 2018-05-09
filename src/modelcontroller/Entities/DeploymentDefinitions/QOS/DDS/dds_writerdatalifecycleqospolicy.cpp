#include "dds_writerdatalifecycleqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE;
const static QString kind_string = "DDS_WriterDataLifecycleQosPolicy";

void DDS_WriterDataLifecycleQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_WriterDataLifecycleQosPolicy(factory, is_temp_node);
    });
}

DDS_WriterDataLifecycleQosPolicy::DDS_WriterDataLifecycleQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "writer_data_lifecycle", true);
    factory.AttachData(this, "qos_dds_autodispose_unregistered_instances", QVariant::Bool, true, false);
}