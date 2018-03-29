#include "header.h"



Header::Header(EntityFactory* factory) : BehaviourNode(factory, NODE_KIND::HEADER, "Header"){
	auto node_kind = NODE_KIND::HEADER;
	QString kind_string = "Header";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Header();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "code", QVariant::String);
    RegisterDefaultData(factory, node_kind, "row", QVariant::Int, true, 2);
    RegisterDefaultData(factory, node_kind, "row_subgroup", QVariant::String, true, "RIGHT");

};

Header::Header():BehaviourNode(NODE_KIND::HEADER){
    setWorkflowReciever(false);
    setWorkflowProducer(false);
}

bool Header::canAdoptChild(Node *child)
{
    return false;
}

bool Header::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
