#include "booleanexpression.h"
#include "../Keys/typekey.h"
#include "../data.h"
#include "inputparameter.h"
#include "../../entityfactory.h"
#include <exception>


const NODE_KIND node_kind = NODE_KIND::BOOLEAN_EXPRESSION;
const QString kind_string = "BooleanExpression";

void MEDEA::BooleanExpression::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        if(is_temp_node){
            return new BooleanExpression();
        }else{
            return new BooleanExpression(factory);
        }
    });

    //Register DefaultData
    /*RegisterDefaultData(factory, node_kind, "kind", QVariant::String, true, kind_string);
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, kind_string);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, true, -1);
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true, "Boolean");
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "");*/
}

//Temp Constructor
MEDEA::BooleanExpression::BooleanExpression() : DataNode(node_kind){
}

MEDEA::BooleanExpression::BooleanExpression(EntityFactory& factory) : BooleanExpression(){
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setLabelFunctional(false);

    setDataReceiver(false);
    setDataProducer(true);


    auto lhs = factory.CreateNode(NODE_KIND::INPUT_PARAMETER);
    auto comparator = factory.CreateNode(NODE_KIND::INPUT_PARAMETER);
    auto rhs = factory.CreateNode(NODE_KIND::INPUT_PARAMETER);

    if(rhs && comparator && lhs){
        if(addChild(lhs) && addChild(comparator) && addChild(rhs)){
            //Set the LHS/Comparator and RHS
            this->lhs_ = lhs;
            this->rhs_ = rhs;
            this->comparator_ = comparator;

            auto key_label = factory.GetKey("label", QVariant::String);
            auto key_icon = factory.GetKey("icon", QVariant::String);
            auto key_icon_prefix = factory.GetKey("icon_prefix", QVariant::String);

            factory.AttachData(rhs, key_label, "rhs", true);
            factory.AttachData(rhs, key_icon, "Variable", true);
            factory.AttachData(rhs, key_icon_prefix, "EntityIcons", true);

            factory.AttachData(comparator, key_label, "==", true);
            factory.AttachData(comparator, key_icon, "BooleanExpression", true);
            factory.AttachData(comparator, key_icon_prefix, "EntityIcons", true);

            QList<QVariant> comparator_types = {"==", ">", "<", ">=", "<=", "!=", "&&", "||"};
            comparator->addKeysValidValues("label", comparator_types);
            //RegisterValidDataValues(factory, node_kind, "comparator", QVariant::String, comparator_types);


            factory.AttachData(lhs, key_label, "lhs", true);
            factory.AttachData(lhs, key_icon, "Variable", true);
            factory.AttachData(lhs, key_icon_prefix, "EntityIcons", true);

            auto data_rhs_value = rhs->getData("value");
            auto data_lhs_value = lhs->getData("value");
            auto data_comparator = comparator->getData("label");
            auto data_label = getData("label");

            //Update Label on data Change
            connect(data_rhs_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
            connect(data_lhs_value, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
            connect(data_comparator, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);

            updateLabel();
            TypeKey::BindInnerAndOuterTypes(lhs, rhs, true);
            return;
        }
    }
    
    factory.DestructEntity(lhs);
    factory.DestructEntity(rhs);
    throw std::invalid_argument("Boolean Expression cannot be constructed");
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
    QString new_label = "invalid expression";
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
/*
void MEDEA::BooleanExpression::updateOutputType(){
    bool is_producer = false;
    
    if(comparator_){
        const QSet<QString> comparators = {">", "<", "==", ">=", "<=", "!="};
        is_producer = comparators.contains(comparator_->getDataValue("label").toString());
    }

    setDataProducer(is_producer);
}*/