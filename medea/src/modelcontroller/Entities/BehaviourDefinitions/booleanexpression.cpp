#include "booleanexpression.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include <exception>


const NODE_KIND node_kind = NODE_KIND::BOOLEAN_EXPRESSION;
const QString kind_string = "Boolean Expression";


void MEDEA::BooleanExpression::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::BooleanExpression(broker, is_temp_node);
        });
}

MEDEA::BooleanExpression::BooleanExpression(::EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp, false){
    //Setup State
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setLabelFunctional(false);
    setDataProducer(true);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }


    //setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "???");
    auto data_value = broker.AttachData(this, KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED);
    
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED, "Boolean");
    
    //Attach Children
    lhs_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    comparator_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    rhs_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);

    //Setup LHS
    broker.AttachData(lhs_, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "lhs");
    broker.AttachData(lhs_, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED, "Variable");
    broker.AttachData(lhs_, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED, "EntityIcons");
    broker.AttachData(lhs_, KeyName::IsGenericParam, QVariant::Bool, ProtectedState::PROTECTED, true);
    broker.AttachData(lhs_, KeyName::IsGenericParamSrc, QVariant::Bool, ProtectedState::PROTECTED, true);

    //Setup Comparator
    comparator_->setDataReceiver(false);
    comparator_->setDataProducer(false);
    auto data_comparator = broker.AttachData(comparator_, KeyName::Label, QVariant::String, ProtectedState::UNPROTECTED);
    data_comparator->addValidValues({"==", ">", "<", ">=", "<=", "!=", "&&", "||"});

    broker.AttachData(comparator_, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED, "circleQuestionDark");
    broker.AttachData(comparator_, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED, "Icons");
    broker.AttachData(comparator_, KeyName::IsGenericParam, QVariant::Bool, ProtectedState::PROTECTED, true);
    broker.AttachData(comparator_, KeyName::Value, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(comparator_, KeyName::EditableKey, QVariant::String, ProtectedState::PROTECTED, KeyName::Label);
    Data::LinkData(comparator_, KeyName::Label, comparator_, KeyName::Value, true);

    broker.RemoveData(comparator_, KeyName::Type);
    broker.RemoveData(comparator_, KeyName::InnerType);
    broker.RemoveData(comparator_, KeyName::OuterType);

    //Setup RHS
    broker.AttachData(rhs_, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "rhs");
    broker.AttachData(rhs_, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED, "Variable");
    broker.AttachData(rhs_, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED, "EntityIcons");
    broker.AttachData(rhs_, KeyName::IsGenericParam, QVariant::Bool, ProtectedState::PROTECTED, true);

    //Bind Value changing
    auto data_rhs_value = rhs_->getData(KeyName::Value);
    auto data_lhs_value = lhs_->getData(KeyName::Value);

    //Update Label on data Change
    connect(data_rhs_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    connect(data_lhs_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    connect(data_comparator, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    connect(data_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    
    updateLabel();
    TypeKey::BindTypes(lhs_, rhs_, true, true);
};

bool MEDEA::BooleanExpression::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
    case NODE_KIND::INPUT_PARAMETER:
        if(getChildrenOfKindCount(child_kind) >= 3){
                return false;
            }
        break;
    default:
        break;
    }

    return DataNode::canAdoptChild(child);
}

void MEDEA::BooleanExpression::updateLabel()
{
    QString new_label = "???";
    
    auto value = getDataValue(KeyName::Value).toString();
    if (value.length()) {
        new_label = value;
    } else {
        if(lhs_ && comparator_ && rhs_){
            auto lhs_value = lhs_->getDataValue(KeyName::Value).toString();
            auto comparator = comparator_->getDataValue(KeyName::Label).toString();
            auto rhs_value = rhs_->getDataValue(KeyName::Value).toString();
            if (lhs_value.length() && rhs_value.length()) {
                new_label = lhs_value + " " + comparator + " " + rhs_value;
            }
        }
    }

    setDataValue(KeyName::Label, new_label);
}

DataNode* MEDEA::BooleanExpression::GetLhs(){
    return lhs_;
}

DataNode* MEDEA::BooleanExpression::GetRhs(){
    return rhs_;
}

DataNode* MEDEA::BooleanExpression::GetComparator(){
    return comparator_;
}