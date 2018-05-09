#include "inputparametergroup.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::INPUT_PARAMETER_GROUP;
const QString kind_string = "InputParameterGroup";

void MEDEA::InputParameterGroup::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::InputParameterGroup(factory, is_temp_node);
    });
}

MEDEA::InputParameterGroup::InputParameterGroup(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setLabelFunctional(false);
    addInstanceKind(NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "Input", false);
    factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "index", QVariant::Int, -1, false);
}

bool MEDEA::InputParameterGroup::canAdoptChild(Node* child)
{
    //Server interface Input Parameter Groups can only have a singular child
    if(getParentNodeKind() == NODE_KIND::SERVER_INTERFACE && getChildrenCount() > 0 ){
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