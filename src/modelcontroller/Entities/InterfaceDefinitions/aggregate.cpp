#include "aggregate.h"
#include "../data.h"

#include "../../edgekinds.h"
#include "../Keys/typekey.h"




Aggregate::Aggregate(EntityFactory* factory) : Node(factory, NODE_KIND::AGGREGATE, "Aggregate"){
	auto node_kind = NODE_KIND::AGGREGATE;
	QString kind_string = "Aggregate";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Aggregate();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "namespace", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "comment", QVariant::String);
};

Aggregate::Aggregate(): Node(NODE_KIND::AGGREGATE)
{
    addInstanceKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsEdgeKind(EDGE_KIND::AGGREGATE, EDGE_DIRECTION::TARGET);
}

bool Aggregate::canAdoptChild(Node *child)
{
    if(!child){
        return false;
    }

    NODE_KIND kind = child->getNodeKind();

    switch(kind){
    case NODE_KIND::ENUM_INSTANCE:
    case NODE_KIND::AGGREGATE_INSTANCE:
    case NODE_KIND::MEMBER:
    case NODE_KIND::VECTOR:
        break;
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool Aggregate::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}



void Aggregate::DataAdded(Data* data){
    Node::DataAdded(data);

    auto key_name = data->getKeyName();

    if(key_name == "label" || key_name == "namespace" || key_name == "type"){
        TypeKey::BindNamespaceAndLabelToType(this, true);
    }
}