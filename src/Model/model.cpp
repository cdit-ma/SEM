#include "model.h"

Model::Model(): Node(NODE_KIND::MODEL)
{
    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String);
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

bool Model::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
