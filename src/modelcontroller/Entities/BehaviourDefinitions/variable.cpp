#include "variable.h"

#include "../../edgekinds.h"
#include "../edge.h"
#include "../data.h"
#include "../Keys/typekey.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::VARIABLE;
const QString kind_string = "Variable";

void Variable::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Variable(factory, is_temp_node);
        });
}

Variable::Variable(EntityFactory& factory, bool is_temp) : DataNode(node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setDataProducer(true);
    setAcceptsNodeKind(NODE_KIND::MEMBER);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VECTOR);
    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::EXTERNAL_TYPE);

    //Setup Data
    factory.AttachData(this, "index", QVariant::Integer, -1, false);
}

bool Variable::canAdoptChild(Node* child)
{
    if(getChildrenCount() > 0){
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