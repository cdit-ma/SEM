#include "returnparametergroupinstance.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE;
const QString kind_string = "ReturnParameterGroupInstance";


void MEDEA::ReturnParameterGroupInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ReturnParameterGroupInstance(factory, is_temp_node);
    });
}

MEDEA::ReturnParameterGroupInstance::ReturnParameterGroupInstance(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setLabelFunctional(false);
    addInstancesDefinitionKind(NODE_KIND::RETURN_PARAMETER_GROUP);
    
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

bool MEDEA::ReturnParameterGroupInstance::canAdoptChild(Node* child)
{
    if(getChildrenCount() > 0 ){
        return false;
    }
    return Node::canAdoptChild(child);
}