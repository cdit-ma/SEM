#include "code.h"

#include "../entityfactory.h"

Code::Code(EntityFactory* factory) : BehaviourNode(factory, NODE_KIND::CODE, "Code"){
	auto node_kind = NODE_KIND::CODE;
	QString kind_string = "Code";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Code();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "code", QVariant::String);
};

Code::Code():BehaviourNode(NODE_KIND::CODE){
    setWorkflowReciever(true);
    setWorkflowProducer(true);
}

bool Code::canAdoptChild(Node *child)
{
    return false;
}

bool Code::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
