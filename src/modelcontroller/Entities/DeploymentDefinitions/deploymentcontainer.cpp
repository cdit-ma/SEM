#include "deploymentcontainer.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"


const NODE_KIND node_kind = NODE_KIND::DEPLOYMENT_CONTAINER;
const QString kind_string = "Deployment Container";

void MEDEA::DeploymentContainer::RegisterWithEntityFactory(EntityFactoryRegistryBroker &broker) {
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DeploymentContainer(broker, is_temp_node);
    });
}

MEDEA::DeploymentContainer::DeploymentContainer(EntityFactoryBroker &broker, bool is_temp_node) :
    Node(broker, node_kind, is_temp_node)
{
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);
    setLabelFunctional(false);

    if(is_temp_node){
        return;
    }

    /* //To be readded once docker deployment is supported in re
    broker.AttachData(this, "is_late_joiner", QVariant::Bool, ProtectedState::PROTECTED, false);
    auto data_docker = broker.AttachData(this, "is_docker", QVariant::Bool, ProtectedState::PROTECTED, false);
    connect(data_docker, &Data::dataChanged, this, &MEDEA::DeploymentContainer::TypeUpdated);
    TypeUpdated();*/

    broker.AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "Icons");
    broker.AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED);
}

bool MEDEA::DeploymentContainer::canAcceptEdge(EDGE_KIND edge_kind, Node *destination) {
    static const QSet<NODE_KIND> accepted_target_kinds({NODE_KIND::HARDWARE_CLUSTER, NODE_KIND::HARDWARE_NODE});

    if(!canCurrentlyAcceptEdgeKind(edge_kind, destination)){
        return false;
    }

    // Disallow connecting to other deployment containers.
    if(edge_kind == EDGE_KIND::DEPLOYMENT){
        if(!accepted_target_kinds.contains(destination->getNodeKind())){
            return false;
        }
    }

    return Node::canAcceptEdge(edge_kind, destination);
}

void MEDEA::DeploymentContainer::TypeUpdated(){
    /* To be readded once docker deployment is supported in re
    auto is_docker = getDataValue("is_docker").toBool();
    getFactoryBroker().AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED, (is_docker ? "docker" : "servers"));
    */
}
