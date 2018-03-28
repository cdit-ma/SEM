#include "function.h"

const NODE_KIND node_kind = NODE_KIND::FUNCTION;
const QString kind_string = "Function";

MEDEA::Function::Function(EntityFactory* factory) : BehaviourNode(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Function();});
};

MEDEA::Function::Function(): BehaviourNode(node_kind)
{
    
}

bool MEDEA::Function::Function::canAdoptChild(Node* child)
{
    return false;
}

bool MEDEA::Function::Function::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
