#include "containernode.h"
#include "../node.h"

ContainerNode::ContainerNode(EntityFactory* factory, NODE_KIND kind, QString kind_str) : Node(factory, kind, kind_str){
    RegisterDefaultData(factory, kind, "comment", QVariant::String);
};

ContainerNode::ContainerNode(NODE_KIND kind) : Node(kind)
{

}

bool ContainerNode::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::OUTEVENTPORT_IMPL:
        case NODE_KIND::BRANCH_STATE:
        case NODE_KIND::VARIABLE:
        case NODE_KIND::WORKER_PROCESS:
        case NODE_KIND::WHILELOOP:
        case NODE_KIND::SETTER:
        case NODE_KIND::CODE:
        case NODE_KIND::HEADER:
            break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}
