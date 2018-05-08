#include "model.h"
#include "../nodekinds.h"
#include "../version.h"
#include "../entityfactory.h"
const NODE_KIND node_kind = NODE_KIND::MODEL;
const QString kind_string = "Model";

void Model::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Model(factory, is_temp_node);
    });
}

Model::Model(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setAsRoot(0);
    setAcceptsNodeKind(NODE_KIND::INTERFACE_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::BEHAVIOUR_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::WORKER_DEFINITIONS);

    //Setup Data
    factory.AttachData(this, "medea_version", QVariant::String, APP_VERSION(), true);
    factory.AttachData(this, "description", QVariant::String, "", true);

    //Attach Children
    factory.ConstructChildNode(*this, NODE_KIND::INTERFACE_DEFINITIONS);
    factory.ConstructChildNode(*this, NODE_KIND::BEHAVIOUR_DEFINITIONS);
    factory.ConstructChildNode(*this, NODE_KIND::DEPLOYMENT_DEFINITIONS);
    factory.ConstructChildNode(*this, NODE_KIND::WORKER_DEFINITIONS);
}

bool Model::canAdoptChild(Node *child)
{
    if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(child);
}