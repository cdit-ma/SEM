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
    NODE_KIND kind = child->getNodeKind();

    auto parent_kind = getParentNodeKind();
    auto is_in_interface = parent_kind == NODE_KIND::SERVER_INTERFACE;

    switch(kind){
    case NODE_KIND::AGGREGATE_INSTANCE:
        break;
    case NODE_KIND::ENUM_INSTANCE:
    case NODE_KIND::MEMBER:
    case NODE_KIND::VECTOR:
        if(is_in_interface){
            return false;
        }
        break;
    default:
        return false;
    }
    
    if(is_in_interface && childrenCount() > 0 ){
        return false;
    }
    
    return Node::canAdoptChild(child);
}
