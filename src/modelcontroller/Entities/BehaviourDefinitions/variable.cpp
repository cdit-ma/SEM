#include "variable.h"

#include "../../edgekinds.h"
#include "../edge.h"
#include "../data.h"
#include "../Keys/typekey.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::VARIABLE;
const QString kind_string = "Variable";

Variable::Variable(EntityFactory* factory) : DataNode(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Variable();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "inner_type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "outer_type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, true);
};

Variable::Variable() : DataNode(node_kind)
{
    setDataProducer(true);
    //setDataReceiver(true);
    //setMultipleDataReceiver(true);
}



bool Variable::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::MEMBER:
    case NODE_KIND::AGGREGATE_INSTANCE:
    case NODE_KIND::VECTOR:
    case NODE_KIND::ENUM_INSTANCE:
    case NODE_KIND::EXTERNAL_TYPE:
        break;
    default:
        return false;
    }
    if(hasChildren()){
        return false;
    }

    return DataNode::canAdoptChild(child);
}

void Variable::childAdded(Node* child){
    DataNode::childAdded(child);
    TypeKey::BindInnerAndOuterTypes(child, this, true);
}

void Variable::childRemoved(Node* child){
    DataNode::childRemoved(child);
    TypeKey::BindInnerAndOuterTypes(child, this, false);
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
