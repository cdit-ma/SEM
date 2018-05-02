#include "dds_entityfactoryqospolicy.h"

DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY, "DDS_EntityFactoryQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY;
	QString kind_string = "DDS_EntityFactoryQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_EntityFactoryQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "entity_factory");
    RegisterDefaultData(factory, node_kind, "qos_dds_autoenable_created_entities", QVariant::Bool, false, true);

};

DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}
