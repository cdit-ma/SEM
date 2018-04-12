#include "whileloop.h"

const NODE_KIND node_kind = NODE_KIND::WHILE_LOOP;
const QString kind_string = "WhileLoop";

MEDEA::WhileLoop::WhileLoop(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WhileLoop();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, "while");
};

MEDEA::WhileLoop::WhileLoop():Node(NODE_KIND::WHILE_LOOP){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
}

bool MEDEA::WhileLoop::canAdoptChild(Node *child)
{
    if(ContainerNode::canAdoptChild(child)){
        return Node::canAdoptChild(child);
    }
    return false;
}

bool MEDEA::WhileLoop::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}


