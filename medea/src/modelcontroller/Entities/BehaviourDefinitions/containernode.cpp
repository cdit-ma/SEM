#include "containernode.h"

QSet<NODE_KIND> ContainerNode::getAcceptedNodeKinds(){
    return {
        NODE_KIND::PORT_PUBLISHER_IMPL,
        NODE_KIND::PORT_REQUESTER_IMPL,
        NODE_KIND::IF_STATEMENT,
        NODE_KIND::FOR_LOOP,
        NODE_KIND::VARIABLE,
        NODE_KIND::WHILE_LOOP,
        NODE_KIND::SETTER,
        NODE_KIND::CODE,
        NODE_KIND::FUNCTION_CALL,
        NODE_KIND::BOOLEAN_EXPRESSION
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