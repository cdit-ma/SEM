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

    //Don't allow late joiner flag to be modified
    broker.AttachData(this, KeyName::IsLateJoiner, QVariant::Bool, ProtectedState::PROTECTED, false);
    auto data_docker = broker.AttachData(this, KeyName::IsDocker, QVariant::Bool, ProtectedState::UNPROTECTED, false);
    connect(data_docker, &Data::dataChanged, this, &MEDEA::DeploymentContainer::TypeUpdated);

    broker.AttachData(this, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED, "Icons");
    broker.AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED);
    TypeUpdated();
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

void MEDEA::DeploymentContainer::TypeUpdated()
{
    auto is_docker = getDataValue(KeyName::IsDocker).toBool();
    getFactoryBroker().AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED, (is_docker ? "docker" : "servers"));
}
