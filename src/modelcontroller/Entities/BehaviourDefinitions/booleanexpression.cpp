#include "booleanexpression.h"
#include "../Keys/typekey.h"
#include "../data.h"
#include "inputparameter.h"
#include "../../entityfactory.h"


const NODE_KIND node_kind = NODE_KIND::BOOLEAN_EXPRESSION;
const QString kind_string = "BooleanExpression";


MEDEA::BooleanExpression::BooleanExpression(EntityFactory* factory) : DataNode(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new BooleanExpression();});
    RegisterComplexNodeKind(factory, node_kind, &ConstructBooleanExpression);


    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "comparator", QVariant::String, false, "==");
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true, "Boolean");
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "");
    RegisterDefaultData(factory, node_kind, "value", QVariant::String, false);
    
    QList<QVariant> comparator_types = {">", "<", "==", ">=", "<=", "!=", "=", "+=", "-=", "*=", "/="};

    RegisterValidDataValues(factory, node_kind, "comparator", QVariant::String, comparator_types);
};

#include <QDebug>
Node* MEDEA::BooleanExpression::ConstructBooleanExpression(EntityFactory* factory){
    //Don't recurse into the complex function
    auto node = factory->CreateNode(NODE_KIND::BOOLEAN_EXPRESSION, false);
    auto lhs = factory->CreateNode(NODE_KIND::INPUT_PARAMETER);
    auto rhs = factory->CreateNode(NODE_KIND::INPUT_PARAMETER);

    if(node && rhs && lhs){
        auto key_icon = factory->GetKey("icon", QVariant::String);
        auto key_icon_prefix = factory->GetKey("icon_prefix", QVariant::String);

        factory->AttachData(rhs, key_icon, "Variable");
        factory->AttachData(rhs, key_icon_prefix, "EntityIcons");

        factory->AttachData(lhs, key_icon, "Variable");
        factory->AttachData(lhs, key_icon_prefix, "EntityIcons");

        if(node->addChild(rhs) && node->addChild(lhs)){
            return node;
        }
    }

    if(node){
        factory->DestructEntity(lhs);
        factory->DestructEntity(rhs);
        factory->DestructEntity(node);
    }
    return 0;
}

MEDEA::BooleanExpression::BooleanExpression() : DataNode(node_kind)
{
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setLabelFunctional(false);

    setDataReceiver(false);
    setDataProducer(false);
}

bool MEDEA::BooleanExpression::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
    case NODE_KIND::INPUT_PARAMETER:
        if(getChildrenOfKindCount(child_kind) >= 2){
                return false;
            }
        break;
    default:
        break;
    }

    return DataNode::canAdoptChild(child);
}

void MEDEA::BooleanExpression::updateLabel(){
    auto input_parameters = getChildrenOfKind(NODE_KIND::INPUT_PARAMETER, 0);
    QString new_label = "invalid expression";
    if(input_parameters.size() == 2){
        auto comparator = getDataValue("comparator").toString();
        auto lhs_value = input_parameters[0]->getDataValue("value").toString();
        auto rhs_value = input_parameters[1]->getDataValue("value").toString();

        if(lhs_value.length() && rhs_value.length()){
            new_label = lhs_value + " " + comparator + " " + rhs_value;
        }
    }
    setDataValue("label", new_label);
}

void MEDEA::BooleanExpression::updateOutputType(){
    auto comparator = getDataValue("comparator").toString();
    QSet<QString> comparators = {">", "<", "==", ">=", "<=", "!="};

    auto is_comparator = comparators.contains(comparator);
    setDataProducer(is_comparator);
}

void MEDEA::BooleanExpression::childAdded(Node* child){
    DataNode::childAdded(child);
    
    auto node_kind = child->getNodeKind();

    if(node_kind == NODE_KIND::INPUT_PARAMETER){
        auto value_data = child->getData("value");
        if(value_data){
            connect(value_data, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
            updateLabel();
        }
        auto input_params = getChildrenOfKind(NODE_KIND::INPUT_PARAMETER);
        if(input_params.length() == 2){
            //Bind types
            TypeKey::BindInnerAndOuterTypes(this, input_params[0], true);
            TypeKey::BindInnerAndOuterTypes(input_params[0], input_params[1], true);
        }
    }
}

void MEDEA::BooleanExpression::DataAdded(Data* data){
    Node::DataAdded(data);

    auto key_name = data->getKeyName();

    if(key_name == "comparator"){
        connect(data, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateLabel);
        connect(data, &Data::dataChanged, this, &MEDEA::BooleanExpression::updateOutputType);
        updateLabel();
        updateOutputType();
    }
}