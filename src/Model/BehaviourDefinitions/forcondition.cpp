#include "forcondition.h"

ForCondition::ForCondition():Condition(NK_FOR_CONDITION){
}

bool ForCondition::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
        case Node::NK_VARIABLE_PARAMETER:
        case Node::NK_INPUT_PARAMETER:
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



