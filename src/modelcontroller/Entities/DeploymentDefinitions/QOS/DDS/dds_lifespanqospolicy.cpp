#include "dds_lifespanqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_LIFESPAN;
const static QString kind_string = "DDS_LifespanQosPolicy";

void DDS_LifespanQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_LifespanQosPolicy(factory, is_temp_node);
    });
}

DDS_LifespanQosPolicy::DDS_LifespanQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "lifespan", true);
    factory.AttachData(this, "qos_dds_duration_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    factory.AttachData(this, "qos_dds_duration_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
}