#include "dds_deadlineqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_DEADLINE;
const static QString kind_string = "DDS_DeadlineQosPolicy";

void DDS_DeadlineQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_DeadlineQosPolicy(factory, is_temp_node);
    });
}

DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "deadline", true);
    factory.AttachData(this, "qos_dds_period_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    factory.AttachData(this, "qos_dds_period_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
}