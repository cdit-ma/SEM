#include "serverinterface.h"
#include "../../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::SERVER_INTERFACE;
const QString kind_string = "ServerInterface";

void MEDEA::ServerInterface::RegisterWithEntityFactory(EntityFactory& factory){
	Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ServerInterface(factory, is_temp_node);
        });
};


MEDEA::ServerInterface::ServerInterface(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    addInstanceKind(NODE_KIND::SERVER_PORT);
    addInstanceKind(NODE_KIND::CLIENT_PORT);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP);

    //Setup Children
    auto input_params = factory.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER_GROUP);
    auto return_params = factory.ConstructChildNode(*this, NODE_KIND::RETURN_PARAMETER_GROUP);

    factory.AttachData(input_params, "label", QVariant::String, "Request", false);
    factory.AttachData(return_params, "label", QVariant::String, "Reply", false);
}


bool MEDEA::ServerInterface::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
    case NODE_KIND::INPUT_PARAMETER_GROUP:
    case NODE_KIND::RETURN_PARAMETER_GROUP:{
        if(getChildrenOfKindCount(child_kind) > 0){
        return false;
    }
        break;
    }
    default:
        return false;
    };
    return Node::canAdoptChild(child);
}