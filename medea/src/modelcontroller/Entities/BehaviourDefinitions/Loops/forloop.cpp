#include "forloop.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../InterfaceDefinitions/datanode.h"
#include "../setter.h"
#include "../booleanexpression.h"
#include "../../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::FOR_LOOP;
const QString kind_string = "For Loop";

void MEDEA::ForLoop::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ForLoop(broker, is_temp_node);
        });
}

MEDEA::ForLoop::ForLoop(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);

    setAcceptsNodeKind(NODE_KIND::VARIABLE_PARAMETER);
    setAcceptsNodeKind(NODE_KIND::BOOLEAN_EXPRESSION);
    setAcceptsNodeKind(NODE_KIND::SETTER);

    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Attach Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);

    //Attach Children
    variable_ = broker.ConstructChildNode(*this, NODE_KIND::VARIABLE_PARAMETER);
    expression_ = (BooleanExpression*) broker.ConstructChildNode(*this, NODE_KIND::BOOLEAN_EXPRESSION);
    iteration_ = (Setter*) broker.ConstructChildNode(*this, NODE_KIND::SETTER);

    //Setup Variable
    broker.AttachData(variable_, KeyName::Label, QVariant::String, ProtectedState::UNPROTECTED, "i");
    broker.AttachData(variable_, KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED, 0);

    auto variable_type = broker.AttachData(variable_, KeyName::Type, QVariant::String, ProtectedState::UNPROTECTED, "Integer");
    variable_type->addValidValues(TypeKey::GetValidNumberTypes());

    //Setup Expression
    expression_->setDataReceiver(true);
    expression_->setDataProducer(false);

    //Set Defaults for the Expression Children
    broker.AttachData(expression_->GetLhs(), KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED, "i");
    broker.AttachData(expression_->GetComparator(), KeyName::Label, QVariant::String, ProtectedState::UNPROTECTED, "<");
    broker.AttachData(expression_->GetRhs(), KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED, "?");

    //Setup Iteration element
    broker.AttachData(iteration_, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED, "reload");
    broker.AttachData(iteration_, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED, "Icons");

    //Set Defaults for the Iteration Children
    broker.AttachData(iteration_->GetLhs(), KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED, "i");
    broker.AttachData(iteration_->GetOperator(), KeyName::Label, QVariant::String, ProtectedState::UNPROTECTED, "+=");
    broker.AttachData(iteration_->GetRhs(), KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED, "1");

    for(auto child : QList<Node*>({variable_, expression_, iteration_})){
        broker.AttachData(child, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 0);
        broker.AttachData(child, KeyName::Column, QVariant::Int, ProtectedState::PROTECTED, -1);
        broker.AttachData(child, KeyName::Index, QVariant::Int, ProtectedState::PROTECTED);
    }

    //Set Comparator/iteration LHS inner_type to match the initial type of i (Integer)
    broker.AttachData(expression_->GetLhs(), KeyName::InnerType, QVariant::String, ProtectedState::PROTECTED, "Integer");
    broker.AttachData(iteration_->GetLhs(), KeyName::InnerType, QVariant::String, ProtectedState::PROTECTED, "Integer");

    //Bind Value changing
    auto data_variable_label = variable_->getData(KeyName::Label);
    auto data_variable_type = variable_->getData(KeyName::Type);
    auto data_variable_value = variable_->getData(KeyName::Value);
    auto data_expression_label = expression_->getData(KeyName::Label);
    auto data_iteration_label = iteration_->getData(KeyName::Label);

    Data::LinkData(variable_, KeyName::Type, expression_->GetLhs(), KeyName::InnerType, true);
    Data::LinkData(variable_, KeyName::Type, iteration_->GetLhs(), KeyName::InnerType, true);

    //Update Label on data Change
    connect(data_variable_label, &Data::dataChanged, this, &MEDEA::ForLoop::updateLabel);
    connect(data_variable_type, &Data::dataChanged, this, &MEDEA::ForLoop::updateLabel);
    connect(data_variable_value, &Data::dataChanged, this, &MEDEA::ForLoop::updateLabel);
    connect(data_expression_label, &Data::dataChanged, this, &MEDEA::ForLoop::updateLabel);
    connect(data_iteration_label, &Data::dataChanged, this, &MEDEA::ForLoop::updateLabel);
    updateLabel();
}

bool MEDEA::ForLoop::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    
    switch(child_kind){
        case NODE_KIND::VARIABLE_PARAMETER:{
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }
        }
        case NODE_KIND::INPUT_PARAMETER:
            if(getChildrenOfKindCount(child_kind) >= 2){
                return false;
            }
            break;
    default:
        break;
    }
    
    //Ignore the can adopt child from condition
    return Node::canAdoptChild(child);
}

void MEDEA::ForLoop::updateLabel(){
    QString new_label = "for";
    if(variable_ && expression_ && iteration_){
        new_label += "(";
        auto var_type = TypeKey::GetCPPPrimitiveType(variable_->getDataValue(KeyName::Type).toString());
        auto var_label = variable_->getDataValue(KeyName::Label).toString();
        auto var_value = variable_->getDataValue(KeyName::Value).toString();
        
        auto expression_label = expression_->getDataValue(KeyName::Label).toString();
        auto itterator_label = iteration_->getDataValue(KeyName::Label).toString();

        new_label += var_type + ' ' + var_label;
        new_label += "=" + var_value + "; ";
        new_label += expression_label + "; ";
        new_label += itterator_label + "";
        new_label += ")";
    }
    setDataValue(KeyName::Label, new_label);
}