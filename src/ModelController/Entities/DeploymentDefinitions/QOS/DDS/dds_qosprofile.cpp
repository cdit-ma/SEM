#include "dds_qosprofile.h"

#include "../../../../edgekinds.h"


DDS_QOSProfile::DDS_QOSProfile(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_PROFILE, "DDS_QOSProfile"){
	auto node_kind = NODE_KIND::QOS_DDS_PROFILE;
	QString kind_string = "DDS_QOSProfile";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_QOSProfile();});
};
DDS_QOSProfile::DDS_QOSProfile():Node(NODE_KIND::QOS_DDS_PROFILE)
{
    setNodeType(NODE_TYPE::QOS_PROFILE);
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);
    setAcceptsEdgeKind(EDGE_KIND::QOS);

    //setMoveEnabled(false);
    //setExpandEnabled(false);
    //updateDefaultData("label", QVariant::String, false, "qos_profile");
}

bool DDS_QOSProfile::canAdoptChild(Node *node)
{
    if(!node->isNodeOfType(NODE_TYPE::QOS)){
        return false;
    }
    if(!node->isNodeOfType(NODE_TYPE::DDS)){
        return false;
    }
    if(node->isNodeOfType(NODE_TYPE::QOS_PROFILE)){
        return false;
    }

    if(!getChildrenOfKind(node->getNodeKind(), 0).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(node);
}

bool DDS_QOSProfile::canAcceptEdge(EDGE_KIND , Node *)
{
    return false;
}