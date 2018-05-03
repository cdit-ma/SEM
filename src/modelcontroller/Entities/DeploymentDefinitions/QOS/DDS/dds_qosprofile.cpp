#include "dds_qosprofile.h"
#include "../../../../edgekinds.h"


DDS_QOSProfile::DDS_QOSProfile(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_PROFILE, "DDS_QOSProfile"){
	auto node_kind = NODE_KIND::QOS_DDS_PROFILE;
	QString kind_string = "DDS_QOSProfile";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_QOSProfile();});
};

DDS_QOSProfile::DDS_QOSProfile():Node(NODE_KIND::QOS_DDS_PROFILE)
{
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::TARGET);
    
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_DEADLINE);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_DESTINATIONORDER);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_DURABILITY);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_GROUPDATA);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_HISTORY);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_LATENCYBUDGET);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_LIFESPAN);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_LIVELINESS);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_OWNERSHIP);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_OWNERSHIPSTRENGTH);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_PARTITION);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_PRESENTATION);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_READERDATALIFECYCLE);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_RELIABILITY);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_RESOURCELIMITS);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_TIMEBASEDFILTER);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_TOPICDATA);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_TRANSPORTPRIORITY);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_USERDATA);
    setAcceptsNodeKind(NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE);
}

bool DDS_QOSProfile::canAdoptChild(Node *node){
    if(!getChildrenOfKind(node->getNodeKind(), 0).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(node);
}