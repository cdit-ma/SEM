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
    switch(child->getNodeKind()){
        case NODE_KIND::INPUT_PARAMETER:
        case NODE_KIND::AGGREGATE_INSTANCE:
            break;
    default:
        return false;
    }

    if(childrenCount() > 0 ){
        return false;
    }
    return Node::canAdoptChild(child);
}