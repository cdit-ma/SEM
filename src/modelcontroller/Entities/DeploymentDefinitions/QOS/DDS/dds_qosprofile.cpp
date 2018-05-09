#include "dds_qosprofile.h"
#include "../../../../edgekinds.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_PROFILE;
const static QString kind_string = "DDS_QOSProfile";

void DDS_QOSProfile::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_QOSProfile(factory, is_temp_node);
    });
}

DDS_QOSProfile::DDS_QOSProfile(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
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
    if(getChildrenOfKindCount(node->getNodeKind()) > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}