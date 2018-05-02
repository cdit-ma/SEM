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

    setAcceptsNodeKind(NODE_KIND::MEMBER);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VECTOR);
    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::EXTERNAL_TYPE);
}



bool Variable::canAdoptChild(Node* child)
{
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