#include "forcondition.h"

ForCondition::ForCondition():Condition(NODE_KIND::FOR_CONDITION){
}

bool ForCondition::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::VARIABLE_PARAMETER:
        case NODE_KIND::INPUT_PARAMETER:
            break;
    default:
        return false;
    }
    //Ignore the can adopt child from condition
    return Node::canAdoptChild(child);
}

bool ForCondition::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Condition::canAcceptEdge(edgeKind, dst);
}



