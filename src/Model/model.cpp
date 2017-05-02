#include "model.h"


Model::Model(EntityFactory* factory) : Node(factory, NODE_KIND::MODEL, "Model"){
	auto node_kind = NODE_KIND::MODEL;
	QString kind_string = "Model";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Model();});

    //Register Data
    //RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
};

Model::Model(): Node(NODE_KIND::MODEL)
{
    setAsRoot(0);
    //setMoveEnabled(false);
    //setExpandEnabled(false);
    //updateDefaultData("label", QVariant::String);
}

bool Model::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::INTERFACE_DEFINITIONS:
    case NODE_KIND::DEPLOYMENT_DEFINITIONS:
    case NODE_KIND::BEHAVIOUR_DEFINITIONS:
        break;
    default:
        return false;
    }



    if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool Model::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
