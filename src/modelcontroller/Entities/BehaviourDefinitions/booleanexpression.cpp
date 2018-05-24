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
    auto label = broker.AttachData(this, "label", QVariant::String, "???", true);
    auto data_value = broker.AttachData(this, "value", QVariant::String, "", false);
    broker.ProtectData(this, "index", false);
    broker.AttachData(this, "type", QVariant::String, "Boolean", true);
    
    //Attach Children
    lhs_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    comparator_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    rhs_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);


    

    //Setup LHS
    broker.AttachData(lhs_, "label", QVariant::String, "lhs", true);
    broker.AttachData(lhs_, "icon", QVariant::String, "Variable", true);
    broker.AttachData(lhs_, "icon_prefix", QVariant::String, "EntityIcons", true);
    broker.AttachData(lhs_, "is_generic_param", QVariant::Bool, true, true);

    //Setup Comparator
    comparator_->setDataReceiver(false);
    comparator_->setDataProducer(false);
    auto data_comparator = broker.AttachData(comparator_, "label", QVariant::String, "==", false);
    data_comparator->addValidValues({"==", ">", "<", ">=", "<=", "!=", "&&", "||"});
    broker.AttachData(comparator_, "icon", QVariant::String, "circleQuestionDark", true);
    broker.AttachData(comparator_, "icon_prefix", QVariant::String, "Icons", true);
    broker.RemoveData(comparator_, "value");
    broker.RemoveData(comparator_, "type");
    broker.RemoveData(comparator_, "inner_type");
    broker.RemoveData(comparator_, "outer_type");

    //Setup RHS
    broker.AttachData(rhs_, "label", QVariant::String, "rhs", true);
    broker.AttachData(rhs_, "icon", QVariant::String, "Variable", true);
    broker.AttachData(rhs_, "icon_prefix", QVariant::String, "EntityIcons", true);
    broker.AttachData(rhs_, "is_generic_param", QVariant::Bool, true, true);

    //Bind Value changing
    auto data_rhs_value = rhs_->getData("value");
    auto data_lhs_value = lhs_->getData("value");

    //Update Label on data Change
    connect(data_rhs_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    connect(data_lhs_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    connect(data_comparator, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    connect(data_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
    

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
    
    auto value = getDataValue("value").toString();
    if(value.length()){
        new_label = value;
    }else{
        if(lhs_ && comparator_ && rhs_){
            auto lhs_value = lhs_->getDataValue("value").toString();
            auto comparator = comparator_->getDataValue("label").toString();
            auto rhs_value = rhs_->getDataValue("value").toString();

            if(lhs_value.length() && rhs_value.length()){
                new_label = lhs_value + " " + comparator + " " + rhs_value;
            }
        }
    }

    setDataValue("label", new_label);
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