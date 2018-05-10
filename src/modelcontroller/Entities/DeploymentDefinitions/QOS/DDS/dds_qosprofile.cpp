#include "dds_qosprofile.h"
#include "../../../../edgekinds.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_PROFILE;
const static QString kind_string = "DDS_QOSProfile";

void DDS_QOSProfile::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_QOSProfile(broker, is_temp_node);
    });
}

DDS_QOSProfile::DDS_QOSProfile(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::TARGET);

    QList<NODE_KIND> adoptable_kinds({
        NODE_KIND::QOS_DDS_POLICY_DEADLINE,
        NODE_KIND::QOS_DDS_POLICY_DESTINATIONORDER,
        NODE_KIND::QOS_DDS_POLICY_DURABILITY,
        NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE,
        NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY,
        NODE_KIND::QOS_DDS_POLICY_GROUPDATA,
        NODE_KIND::QOS_DDS_POLICY_HISTORY,
        NODE_KIND::QOS_DDS_POLICY_LATENCYBUDGET,
        NODE_KIND::QOS_DDS_POLICY_LIFESPAN,
        NODE_KIND::QOS_DDS_POLICY_LIVELINESS,
        NODE_KIND::QOS_DDS_POLICY_OWNERSHIP,
        NODE_KIND::QOS_DDS_POLICY_OWNERSHIPSTRENGTH,
        NODE_KIND::QOS_DDS_POLICY_PARTITION,
        NODE_KIND::QOS_DDS_POLICY_PRESENTATION,
        NODE_KIND::QOS_DDS_POLICY_READERDATALIFECYCLE,
        NODE_KIND::QOS_DDS_POLICY_RELIABILITY,
        NODE_KIND::QOS_DDS_POLICY_RESOURCELIMITS,
        NODE_KIND::QOS_DDS_POLICY_TIMEBASEDFILTER,
        NODE_KIND::QOS_DDS_POLICY_TOPICDATA,
        NODE_KIND::QOS_DDS_POLICY_TRANSPORTPRIORITY,
        NODE_KIND::QOS_DDS_POLICY_USERDATA,
        NODE_KIND::QOS_DDS_POLICY_WRITERDATALIFECYCLE
    });

    //Register Valid Node Kinds
    for(auto adoptable_kind : adoptable_kinds){
        setAcceptsNodeKind(adoptable_kind);
    }

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Add Children
    for(auto adoptable_kind : adoptable_kinds){
        broker.ConstructChildNode(*this, adoptable_kind);
    }
}

bool DDS_QOSProfile::canAdoptChild(Node *node){
    if(getChildrenOfKindCount(node->getNodeKind()) > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}