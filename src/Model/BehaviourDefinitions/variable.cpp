#include "variable.h"

//#include "../entityfactory.h"

Variable::Variable(EntityFactory* factory) : DataNode(factory, NODE_KIND::VARIABLE, "Variable"){
	auto node_kind = NODE_KIND::VARIABLE;
	QString kind_string = "Variable";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Variable();});

     //Register DefaultData
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "String");
    RegisterDefaultData(factory, node_kind, "value", QVariant::String);
};
Variable::Variable():DataNode(NODE_KIND::VARIABLE)
{
    setDataProducer(true);
    setDataReciever(true);
    setMultipleDataReceiver(true);
}

bool Variable::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::AGGREGATE_INSTANCE:
    case NODE_KIND::VECTOR:
        break;
    default:
        return false;
    }
    if(hasChildren()){
        return false;
    }

    return DataNode::canAdoptChild(child);
}

bool Variable::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case EDGE_KIND::DATA:{
        if(dst->getNodeKind() == NODE_KIND::VARIABLE || dst->getNodeKind() == NODE_KIND::ATTRIBUTE_IMPL){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}
