#include "returnparametergroupinstance.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE;
const QString kind_string = "ReturnParameterGroupInstance";

MEDEA::ReturnParameterGroupInstance::ReturnParameterGroupInstance(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ReturnParameterGroupInstance();});
};

MEDEA::ReturnParameterGroupInstance::ReturnParameterGroupInstance(): Node(node_kind)
{
    addInstancesDefinitionKind(NODE_KIND::RETURN_PARAMETER_GROUP);
    setChainableDefinition();
    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VOID_TYPE);
}


bool MEDEA::ReturnParameterGroupInstance::canAdoptChild(Node* child)
{
    if(childrenCount() > 0 ){
        return false;
    }
    return Node::canAdoptChild(child);
}