#include "returnparametergroup.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER_GROUP;
const QString kind_string = "ReturnParameterGroup";

void MEDEA::ReturnParameterGroup::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ReturnParameterGroup(factory, is_temp_node);
    });
}

MEDEA::ReturnParameterGroup::ReturnParameterGroup(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstanceKind(NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE);

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "index", QVariant::Int, -1, false);
}

bool MEDEA::ReturnParameterGroup::canAdoptChild(Node* child)
{ 
    //Server interface Input Parameter Groups can only have a singular child
    if(getChildrenCount() > 0 ){
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