#include "dds_presentationqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_PRESENTATION;
const static QString kind_string = "DDS_PresentationQosPolicy";

void DDS_PresentationQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_PresentationQosPolicy(factory, is_temp_node);
    });
}

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    QList<QVariant> values;
    values << "INSTANCE_PRESENTATION_QOS";
    values << "TOPIC_PRESENTATION_QOS";
    values << "GROUP_PRESENTATION_QOS";
    factory.AttachData(this, "label", QVariant::String, "presentation", true);
    factory.AttachData(this, "qos_dds_coherent_access", QVariant::Bool, false, false);
    factory.AttachData(this, "qos_dds_ordered_access", QVariant::Bool, false, false);
    auto dds_scope_data = factory.AttachData(this, "qos_dds_access_scope", QVariant::String, values.first(), false);
    dds_scope_data->addValidValues(values);
}