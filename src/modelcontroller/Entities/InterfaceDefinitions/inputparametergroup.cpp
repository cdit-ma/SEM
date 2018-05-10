#include "inputparametergroup.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::INPUT_PARAMETER_GROUP;
const QString kind_string = "InputParameterGroup";

void MEDEA::InputParameterGroup::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::InputParameterGroup(broker, is_temp_node);
    });
}

MEDEA::InputParameterGroup::InputParameterGroup(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    addInstanceKind(NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "Input", false);
    broker.AttachData(this, "type", QVariant::String, "", true);
    broker.AttachData(this, "index", QVariant::Int, -1, false);
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