#include "callbackfunction.h"
#include "../data.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::CALLBACK_FUNCTION;
const QString kind_string = "CallbackFunction";

void MEDEA::CallbackFunction::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::CallbackFunction(broker, is_temp_node);
        });
}

MEDEA::CallbackFunction::CallbackFunction(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    addInstanceKind(NODE_KIND::CALLBACK_FUNCTION_INST);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    setLabelFunctional(false);

    //Setup Data
    broker.AttachData(this, "class", QVariant::String, ProtectedState::PROTECTED);

    broker.AttachData(this, "operation", QVariant::String, ProtectedState::UNPROTECTED, "Function");

    broker.AttachData(this, "icon_prefix", QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "icon", QVariant::String, ProtectedState::UNPROTECTED);
}


bool MEDEA::CallbackFunction::CallbackFunction::canAdoptChild(Node* child)
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
            break;
    }
    return Node::canAdoptChild(child);
}
