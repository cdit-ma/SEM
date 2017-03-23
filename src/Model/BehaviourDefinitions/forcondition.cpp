#include "forcondition.h"

ForCondition::ForCondition():Condition(NK_FOR_CONDITION){
}

bool ForCondition::canAdoptChild(Node*)
{
    return true;
}

bool ForCondition::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Condition::canAcceptEdge(edgeKind, dst);
}



