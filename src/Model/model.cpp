#include "model.h"

Model::Model(): Node(NK_MODEL)
{
    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String);
}

bool Model::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NK_INTERFACE_DEFINITIONS:
    case NK_DEPLOYMENT_DEFINITIONS:
    case NK_BEHAVIOUR_DEFINITIONS:
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
