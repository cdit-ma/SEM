#include "booleanexpression.h"
#include "../Keys/typekey.h"
#include "../../entityfactory.h"
#include <exception>


const NODE_KIND node_kind = NODE_KIND::BOOLEAN_EXPRESSION;
const QString kind_string = "BooleanExpression";


void MEDEA::BooleanExpression::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::BooleanExpression(factory, is_temp_node);
        });
}

MEDEA::BooleanExpression::BooleanExpression(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setLabelFunctional(false);

    
    setDataProducer(true);
    
    //setup Data
    auto label = factory.AttachData(lhs_, "label", QVariant::String, "???", true);
    factory.AttachData(lhs_, "type", QVariant::String, "Boolean", true);
    
    //Attach Children
    lhs_ = factory.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    comparator_ = factory.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    rhs_ = factory.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);

    //Setup LHS
    factory.AttachData(lhs_, "label", QVariant::String, "lhs", true);
    factory.AttachData(lhs_, "icon", QVariant::String, "Variable", true);
    factory.AttachData(lhs_, "icon_prefix", QVariant::String, "EntityIcons", true);

    //Setup Comparator
    auto data_comparator = factory.AttachData(comparator_, "label", QVariant::String, "==", false);
    factory.AttachData(comparator_, "icon", QVariant::String, "circleQuestionDark", true);
    factory.AttachData(comparator_, "icon_prefix", QVariant::String, "Icons", true);
    data_comparator->addValidValues({"==", ">", "<", ">=", "<=", "!=", "&&", "||"});

    //Setup RHS
    factory.AttachData(rhs_, "label", QVariant::String, "rhs", true);
    factory.AttachData(rhs_, "icon", QVariant::String, "Variable", true);
    factory.AttachData(rhs_, "icon_prefix", QVariant::String, "EntityIcons", true);

    //Bind Value changing
    auto data_rhs_value = rhs_->getData("value");
    auto data_lhs_value = lhs_->getData("value");

    //Update Label on data Change
    connect(data_rhs_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    connect(data_lhs_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    connect(data_comparator, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);

    updateLabel();
    TypeKey::BindInnerAndOuterTypes(lhs_, rhs_, true);
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

void MEDEA::BooleanExpression::updateLabel(){
    QString new_label = "???";
    if(lhs_ && comparator_ && rhs_){
        auto lhs_value = lhs_->getDataValue("value").toString();
        auto comparator = comparator_->getDataValue("label").toString();
        auto rhs_value = rhs_->getDataValue("value").toString();

        if(lhs_value.length() && rhs_value.length()){
            new_label = lhs_value + " " + comparator + " " + rhs_value;
        }
    }
    setDataValue("label", new_label);
}