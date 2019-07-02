#include "transitionfunction.h"
#include "../data.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::TRANSITION_FUNCTION;
const QString kind_string = "TransitionFunction";

void MEDEA::TransitionFunction::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::TransitionFunction(broker, is_temp_node);
        });
}

MEDEA::TransitionFunction::TransitionFunction(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

    setAcceptsNodeKind(NODE_KIND::PORT_SUBSCRIBER_IMPL, false);
    setAcceptsNodeKind(NODE_KIND::PORT_PUBLISHER_IMPL, false);
    setAcceptsNodeKind(NODE_KIND::PORT_REQUESTER_IMPL, false);


    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    setLabelFunctional(false);

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED);
    auto data_type = broker.AttachData(this, "type", QVariant::String, ProtectedState::UNPROTECTED);
    data_type->addValidValues({"Configure", "Activate", "Passivate", "Terminate"});

    connect(data_type, &Data::dataChanged, this, &MEDEA::TransitionFunction::updateLabel);
    updateLabel();
}


void MEDEA::TransitionFunction::updateLabel(){
    /*
    QString new_label = "On";
    new_label += getDataValue("type").toString();
    setDataValue("label", new_label + "d");
    */
    QString new_label = "Handle";
    new_label += getDataValue("type").toString();
    setDataValue("label", new_label);
}


bool MEDEA::TransitionFunction::canAdoptChild(Node* child)
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

void MEDEA::TransitionFunction::parentSet(Node* parent){

    auto src_data = parent->getData("label");
    auto dst_data = getData("class");
    if(src_data && dst_data){
        src_data->linkData(dst_data, true);
    }

    

    auto parent_node_kind = parent->getNodeKind();
    if(parent_node_kind != NODE_KIND::COMPONENT_IMPL){
        setAcceptsNodeKind(NODE_KIND::PORT_SUBSCRIBER_IMPL, false);
        setAcceptsNodeKind(NODE_KIND::PORT_PUBLISHER_IMPL, false);
        setAcceptsNodeKind(NODE_KIND::PORT_REQUESTER_IMPL, false);
    }

    if(parent->getViewAspect() == VIEW_ASPECT::BEHAVIOUR && parent_node_kind != NODE_KIND::CLASS_INST){
        auto operation = getData("operation");
        auto label = getData("label");
        if(operation && label){
            operation->linkData(label, true);
        }
    }

    Node::parentSet(parent);
}
