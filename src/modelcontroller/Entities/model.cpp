#include "model.h"
#include "../nodekinds.h"
#include "../version.h"

Model::Model(EntityFactory* factory) : Node(factory, NODE_KIND::MODEL, "Model"){
	auto node_kind = NODE_KIND::MODEL;
	QString kind_string = "Model";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Model();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "medea_version", QVariant::String, true, APP_VERSION());
    RegisterDefaultData(factory, node_kind, "description", QVariant::String);
};

Model::Model(): Node(NODE_KIND::MODEL)
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

bool Model::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
