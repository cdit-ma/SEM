#include "inputparametergroup.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::INPUT_PARAMETER_GROUP;
const QString kind_string = "InputParameterGroup";

MEDEA::InputParameterGroup::InputParameterGroup(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InputParameterGroup();});
};

MEDEA::InputParameterGroup::InputParameterGroup(): Node(node_kind)
{
    addInstanceKind(NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE);
}


bool MEDEA::InputParameterGroup::canAdoptChild(Node* child)
{
    //Server interface Input Parameter Groups can only have a singular child
    if(getParentNodeKind() == NODE_KIND::SERVER_INTERFACE && childrenCount() > 0 ){
        return false;
    }
    return Node::canAdoptChild(child);
}

void MEDEA::InputParameterGroup::parentSet(Node* parent){
    auto parent_kind = parent->getNodeKind();
    if(parent_kind == NODE_KIND::SERVER_INTERFACE){
        //Only allow AggregateInstances
        setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
        setAcceptsNodeKind(NODE_KIND::VOID_TYPE);
    }else{
        setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
        setAcceptsNodeKind(NODE_KIND::VOID_TYPE);
        setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
        setAcceptsNodeKind(NODE_KIND::MEMBER);
        setAcceptsNodeKind(NODE_KIND::VECTOR);
    }
}