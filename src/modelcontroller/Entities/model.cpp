#include "model.h"
#include "../nodekinds.h"
#include "../version.h"
#include "../entityfactory.h"
const NODE_KIND node_kind = NODE_KIND::MODEL;
const QString kind_string = "Model";

Model::Model(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Model();});
    RegisterComplexNodeKind(factory, node_kind, &Model::ConstructModel);

    //Register Data
    RegisterDefaultData(factory, node_kind, "medea_version", QVariant::String, true, APP_VERSION());
    RegisterDefaultData(factory, node_kind, "description", QVariant::String);
};

Model::Model() : Node(node_kind)
{
    setAsRoot(0);
    setAcceptsNodeKind(NODE_KIND::INTERFACE_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::BEHAVIOUR_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::WORKER_DEFINITIONS);
}

bool Model::canAdoptChild(Node *child)
{
    if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(child);
}

Node* Model::ConstructModel(EntityFactory* factory){
    //Don't recurse into the complex function
    auto model = factory->CreateNode(NODE_KIND::MODEL, false);
    auto interface = factory->CreateNode(NODE_KIND::INTERFACE_DEFINITIONS);
    auto behaviour = factory->CreateNode(NODE_KIND::BEHAVIOUR_DEFINITIONS);
    auto deployment = factory->CreateNode(NODE_KIND::DEPLOYMENT_DEFINITIONS);
    auto workers = factory->CreateNode(NODE_KIND::WORKER_DEFINITIONS);

    if(model && interface && behaviour && deployment && workers){
        auto adopt_success = model->addChild(interface);
        adopt_success = adopt_success && model->addChild(behaviour);
        adopt_success = adopt_success && model->addChild(deployment);
        adopt_success = adopt_success && model->addChild(workers);
        
        if(adopt_success){
            return model;
        }
    }

    factory->DestructEntity(model);
    factory->DestructEntity(interface);
    factory->DestructEntity(behaviour);
    factory->DestructEntity(deployment);
    factory->DestructEntity(workers);
    return 0;
}