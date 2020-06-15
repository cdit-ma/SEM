#include "callbackfunction.h"
#include "../data.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::CALLBACK_FNC;
const QString kind_string = "Callback Function";

void MEDEA::CallbackFunction::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::CallbackFunction(broker, is_temp_node);
        });
}

MEDEA::CallbackFunction::CallbackFunction(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    setChainableDefinition();
    addInstanceKind(NODE_KIND::CALLBACK_FNC_INST);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    setLabelFunctional(false);

    //Setup Data
    broker.AttachData(this, KeyName::Class, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Operation, QVariant::String, ProtectedState::UNPROTECTED, "Function");
    broker.AttachData(this, KeyName::IconPrefix, QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::UNPROTECTED);
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

QSet<Node*> MEDEA::CallbackFunction::getDependants() const{
    static std::function<QSet<Node*> (const Node*)> getRecursiveInstances = [](const Node* node){
        QSet<Node*> instances;

        for(auto child : node->getInstances()){
            if(!instances.contains(child)){
                instances += child;
                instances += getRecursiveInstances(child);
            }
        }
        return instances;
    };

    return getRecursiveInstances(this);
}

void MEDEA::CallbackFunction::parentSet(Node* parent){

    auto src_data = parent->getData(KeyName::Label);
    auto dst_data = getData(KeyName::Class);
    if(src_data && dst_data){
        src_data->linkData(dst_data, true);
    }
}