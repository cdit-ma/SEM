#include "model.h"
#include "sem_version.hpp"
#include "../nodekinds.h"
#include "../entityfactorybroker.h"
#include "../entityfactoryregistrybroker.h"
#include "../entityfactoryregistrybroker.h"
const static NODE_KIND node_kind = NODE_KIND::MODEL;
const static QString kind_string = "Model";

void Model::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Model(broker, is_temp_node);
    });
}

Model::Model(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setAsRoot(0);
    setAcceptsNodeKind(NODE_KIND::INTERFACE_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::BEHAVIOUR_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::WORKER_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::TRIGGER_DEFINITIONS);

    if(is_temp){
        return;
    }
 
    //Setup Data
    broker.AttachData(this, KeyName::MedeaVersion, QVariant::String, ProtectedState::PROTECTED,
                      QString::fromStdString(std::string{SEM::GetVersion()}));
    broker.AttachData(this, KeyName::Description, QVariant::String, ProtectedState::PROTECTED);

    //Attach Children
    broker.ConstructChildNode(*this, NODE_KIND::INTERFACE_DEFINITIONS);
    broker.ConstructChildNode(*this, NODE_KIND::BEHAVIOUR_DEFINITIONS);
    broker.ConstructChildNode(*this, NODE_KIND::DEPLOYMENT_DEFINITIONS);
    broker.ConstructChildNode(*this, NODE_KIND::WORKER_DEFINITIONS);
    broker.ConstructChildNode(*this, NODE_KIND::TRIGGER_DEFINITIONS);
}

bool Model::canAdoptChild(Node *child)
{
    if(!getChildrenOfKind(child->getNodeKind()).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(child);
}