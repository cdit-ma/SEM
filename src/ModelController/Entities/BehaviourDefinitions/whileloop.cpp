#include "whileloop.h"

#include "../../edgekinds.h"


WhileLoop::WhileLoop(EntityFactory* factory) : Branch(factory, NODE_KIND::WHILELOOP, "WhileLoop"){
	auto node_kind = NODE_KIND::WHILELOOP;
	QString kind_string = "WhileLoop";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WhileLoop();});
};

WhileLoop::WhileLoop():Branch(NODE_KIND::WHILELOOP){
    
}

bool WhileLoop::canAdoptChild(Node *node)
{
    //Only allow one type.
    if(hasChildren()){
        return false;
    }
    return Branch::canAdoptChild(node);
}

bool WhileLoop::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Branch::canAcceptEdge(edgeKind, dst);
}


