#include "dds_presentationqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_PRESENTATION;
const static QString kind_string = "DDS_PresentationQosPolicy";

void DDS_PresentationQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_PresentationQosPolicy(broker, is_temp_node);
    });
}

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
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
    broker.AttachData(this, "label", QVariant::String, "presentation", true);
    broker.AttachData(this, "qos_dds_coherent_access", QVariant::Bool, false, false);
    broker.AttachData(this, "qos_dds_ordered_access", QVariant::Bool, false, false);
    auto dds_scope_data = broker.AttachData(this, "qos_dds_access_scope", QVariant::String, values.first(), false);
    dds_scope_data->addValidValues(values);
}