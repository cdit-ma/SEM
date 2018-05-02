#include "returnparametergroup.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER_GROUP;
const QString kind_string = "ReturnParameterGroup";

MEDEA::ReturnParameterGroup::ReturnParameterGroup(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ReturnParameterGroup();});
};

MEDEA::ReturnParameterGroup::ReturnParameterGroup(): Node(node_kind)
{
    addInstanceKind(NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE);
}


bool MEDEA::ReturnParameterGroup::canAdoptChild(Node* child)
{ 
    //Server interface Input Parameter Groups can only have a singular child
    if(childrenCount() > 0 ){
        return false;
    }
    return Node::canAdoptChild(child);
}

void MEDEA::ReturnParameterGroup::parentSet(Node* parent){
    auto parent_kind = parent->getNodeKind();
    if(parent_kind == NODE_KIND::SERVER_INTERFACE){
        //Only allow AggregateInstances
        setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
        setAcceptsNodeKind(NODE_KIND::VOID_TYPE);
    }else{
        setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
        setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
        setAcceptsNodeKind(NODE_KIND::MEMBER);
        setAcceptsNodeKind(NODE_KIND::VECTOR);
        setAcceptsNodeKind(NODE_KIND::VOID_TYPE);
    }
}