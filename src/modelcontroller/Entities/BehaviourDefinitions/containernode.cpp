#include "containernode.h"

bool ContainerNode::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::OUTEVENTPORT_IMPL:
        case NODE_KIND::IF_STATEMENT:
        case NODE_KIND::FOR_LOOP:
        case NODE_KIND::VARIABLE:
        case NODE_KIND::WORKER_PROCESS:
        case NODE_KIND::WHILE_LOOP:
        case NODE_KIND::SETTER:
        case NODE_KIND::CODE:
        case NODE_KIND::WORKER_FUNCTIONCALL:
        
        {
            return true;
        }
    default:
        return false;
    }
}
