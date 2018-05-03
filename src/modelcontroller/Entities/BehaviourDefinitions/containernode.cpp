#include "containernode.h"

QSet<NODE_KIND> ContainerNode::getAcceptedNodeKinds(){
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

Node* ContainerNode::getTopBehaviourContainer(Node* node){
    Node* container_node = 0;
    if(node){
        for(auto parent_node : node->getParentNodes(-1)){
            if(parent_node->isNodeOfType(NODE_TYPE::TOP_BEHAVIOUR_CONTAINER)){
                container_node = parent_node;
            }
        }

    }
    return container_node;
}