#include "header.h"

#include "../entityfactory.h"

Header::Header(EntityFactory* factory) : BehaviourNode(factory, NODE_KIND::HEADER, "Header"){
	auto node_kind = NODE_KIND::HEADER;
	QString kind_string = "Header";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Header();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "header_name", QVariant::String);
    RegisterDefaultData(factory, node_kind, "include_path", QVariant::String);
};

Header::Header():BehaviourNode(NODE_KIND::HEADER){
    setWorkflowReciever(false);
    setWorkflowProducer(false);
}

bool Header::canAdoptChild(Node *child)
{
    return false;
}

bool Header::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
