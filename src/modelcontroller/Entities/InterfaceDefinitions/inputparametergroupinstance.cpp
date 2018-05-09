#include "inputparametergroupinstance.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE;
const QString kind_string = "InputParameterGroupInstance";


void MEDEA::InputParameterGroupInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::InputParameterGroupInstance(factory, is_temp_node);
    });
}

MEDEA::InputParameterGroupInstance::InputParameterGroupInstance(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setLabelFunctional(false);
    addInstancesDefinitionKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VOID_TYPE);

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "index", QVariant::Int, -1, false);
}