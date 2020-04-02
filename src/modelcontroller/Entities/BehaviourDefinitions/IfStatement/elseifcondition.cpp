#include "elseifcondition.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../InterfaceDefinitions/datanode.h"

const NODE_KIND node_kind = NODE_KIND::ELSEIF_CONDITION;
const QString kind_string = "Else If Condition";

void MEDEA::ElseIfCondition::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ElseIfCondition(broker, is_temp_node);
        });
}

MEDEA::ElseIfCondition::ElseIfCondition(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    
    setLabelFunctional(false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "else if");
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED, "Boolean");
    broker.AttachData(this, KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED);

    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 0);
    broker.AttachData(this, KeyName::Column, QVariant::Int, ProtectedState::PROTECTED, 0);

    auto expression = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::BOOLEAN_EXPRESSION);
    expression_ = expression;
    broker.AttachData(expression, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 0);
    broker.AttachData(expression, KeyName::Column, QVariant::Int, ProtectedState::PROTECTED, -1);
    broker.AttachData(expression, KeyName::Index, QVariant::Int, ProtectedState::PROTECTED);

    //Set that the Expression can accept data
    expression->setDataReceiver(true);
    expression->setDataProducer(false);

    auto data_expression_label = expression_->getData(KeyName::Label);
    connect(data_expression_label, &Data::dataChanged, this, &MEDEA::ElseIfCondition::updateLabel);
    updateLabel();
}



void MEDEA::ElseIfCondition::updateLabel(){
    QString new_label = "else if";
    if(expression_){
        new_label += "(";
        auto expression_label = expression_->getDataValue(KeyName::Label).toString();
        new_label += expression_label;
        new_label += ")";
    }
    setDataValue(KeyName::Label, new_label);
}