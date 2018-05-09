#include "dds_entityfactoryqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY;
const static QString kind_string = "DDS_EntityFactoryQosPolicy";

void DDS_EntityFactoryQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_EntityFactoryQosPolicy(factory, is_temp_node);
    });
}

DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "entity_factory", true);
    factory.AttachData(this, "qos_dds_autoenable_created_entities", QVariant::Bool, true, false);
}