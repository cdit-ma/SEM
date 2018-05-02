#include "containernode.h"

QSet<NODE_KIND> ContainerNode::getAcceptedNodeKinds() const{
    return {
        NODE_KIND::OUTEVENTPORT_IMPL,
        NODE_KIND::SERVER_REQUEST,
        NODE_KIND::IF_STATEMENT,
        NODE_KIND::FOR_LOOP,
        NODE_KIND::VARIABLE,
        NODE_KIND::WHILE_LOOP,
        NODE_KIND::SETTER,
        NODE_KIND::CODE,
        NODE_KIND::FUNCTION_CALL
    };
}
