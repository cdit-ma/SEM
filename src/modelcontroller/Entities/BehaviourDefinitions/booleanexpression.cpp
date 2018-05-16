#include "booleanexpression.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include <exception>


const NODE_KIND node_kind = NODE_KIND::BOOLEAN_EXPRESSION;
const QString kind_string = "BooleanExpression";


void MEDEA::BooleanExpression::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::BooleanExpression(broker, is_temp_node);
        });
}

MEDEA::BooleanExpression::BooleanExpression(::EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
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
    lhs_ = broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    auto datanode_comparator = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    datanode_comparator->setDataReceiver(false);
    datanode_comparator->setDataProducer(false);
    rhs_ = broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);

    comparator_ = datanode_comparator;
    

    //Setup LHS
    broker.AttachData(lhs_, "label", QVariant::String, "lhs", true);
    broker.AttachData(lhs_, "icon", QVariant::String, "Variable", true);
    broker.AttachData(lhs_, "icon_prefix", QVariant::String, "EntityIcons", true);

    //Setup Comparator
    auto data_comparator = broker.AttachData(comparator_, "label", QVariant::String, "==", false);
    broker.AttachData(comparator_, "icon", QVariant::String, "circleQuestionDark", true);
    broker.AttachData(comparator_, "icon_prefix", QVariant::String, "Icons", true);
    data_comparator->addValidValues({"==", ">", "<", ">=", "<=", "!=", "&&", "||"});

    //Setup RHS
    broker.AttachData(rhs_, "label", QVariant::String, "rhs", true);
    broker.AttachData(rhs_, "icon", QVariant::String, "Variable", true);
    broker.AttachData(rhs_, "icon_prefix", QVariant::String, "EntityIcons", true);

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