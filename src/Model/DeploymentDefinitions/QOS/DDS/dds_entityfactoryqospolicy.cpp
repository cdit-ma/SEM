#include "dds_entityfactoryqospolicy.h"

#include "../../../entityfactory.h"
DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY, "DDS_EntityFactoryQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY;
	QString kind_string = "DDS_EntityFactoryQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_EntityFactoryQosPolicy();});
};
DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "entity_factory");
    updateDefaultData("qos_dds_autoenable_created_entities", QVariant::Bool, false, true);
}

bool DDS_EntityFactoryQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_EntityFactoryQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}