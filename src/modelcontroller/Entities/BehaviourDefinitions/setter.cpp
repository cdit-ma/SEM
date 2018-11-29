#include "setter.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../InterfaceDefinitions/datanode.h"

const NODE_KIND node_kind = NODE_KIND::SETTER;
const QString kind_string = "Setter";

const QSet<QString> operators = {
    "=",
    "+=", "-=", "*=", "/=", //Modifying Operators
    "+", "-", "*", "/" //Inline Operators
};

void Setter::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Setter(broker, is_temp_node);
        });
}

Setter::Setter(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);

    setDataReceiver(false);
    setDataProducer(false);

    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }


    //setup Data
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);

    //Attach Children
    lhs_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    operator_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    rhs_ = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);

    operator_->setDataReceiver(false);
    operator_->setDataProducer(false);


    //Setup LHS
    broker.AttachData(lhs_, "label", QVariant::String, ProtectedState::PROTECTED, "lhs");
    broker.AttachData(lhs_, "icon", QVariant::String, ProtectedState::PROTECTED, "Variable");
    broker.AttachData(lhs_, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "EntityIcons");
    broker.AttachData(lhs_, "is_generic_param", QVariant::Bool, ProtectedState::PROTECTED, true);
    broker.AttachData(lhs_, "is_generic_param_src", QVariant::Bool, ProtectedState::PROTECTED, true);


    //Setup Comparator
    auto data_operator = broker.AttachData(operator_, "label", QVariant::String, ProtectedState::UNPROTECTED);
    QList<QVariant> operator_list;
    for(const auto& o : operators){
        operator_list.push_back(o);
    }
    data_operator->addValidValues(operator_list);

    broker.AttachData(operator_, "icon", QVariant::String, ProtectedState::PROTECTED, "circlePlusDark");
    broker.AttachData(operator_, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "Icons");
    broker.AttachData(operator_, "is_generic_param", QVariant::Bool, ProtectedState::PROTECTED, true);
    broker.AttachData(operator_, "value", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(operator_, "editable_key", QVariant::String, ProtectedState::PROTECTED, "label");

    Data::LinkData(operator_, "label", operator_, "value", true);

    broker.RemoveData(operator_, "type");
    broker.RemoveData(operator_, "inner_type");
    broker.RemoveData(operator_, "outer_type");

    //Setup RHS
    broker.AttachData(rhs_, "label", QVariant::String, ProtectedState::PROTECTED, "rhs");
    broker.AttachData(rhs_, "icon", QVariant::String, ProtectedState::PROTECTED, "Variable");
    broker.AttachData(rhs_, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "EntityIcons");
    broker.AttachData(rhs_, "is_generic_param", QVariant::Bool, ProtectedState::PROTECTED, true);

    //Bind Value changing
    auto data_rhs_value = rhs_->getData("value");
    auto data_lhs_value = lhs_->getData("value");

    //Update Label on data Change
    connect(data_rhs_value, &Data::dataChanged, this, &Setter::updateLabel);
    connect(data_lhs_value, &Data::dataChanged, this, &Setter::updateLabel);
    connect(data_operator, &Data::dataChanged, this, &Setter::operatorChanged);
    connect(data_operator, &Data::dataChanged, this, &Setter::updateLabel);

    updateLabel();
    TypeKey::BindInnerAndOuterTypes(lhs_, rhs_, true);
    TypeKey::BindInnerAndOuterTypes(lhs_, this, true);
    TypeKey::BindInnerAndOuterTypes(rhs_, this, true);
}

bool Setter::canAdoptChild(Node* child)
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

    return Node::canAdoptChild(child);
}

void Setter::operatorChanged(){
    if(operator_){
        auto operator_value = operator_->getDataValue("label").toString();
        QSet<QString> producers = {"+", "-", "*", "/"};

        bool valid_producer = producers.contains(operator_value);

        setDataProducer(valid_producer);
    }
}

QString GetWrappedString(const QString& str){
    const QSet<QString> spaced_operators = {" + ", " - ", " * ", " / " /*Inline Operators*/};

    bool wrap = false;
    for(const auto& op : spaced_operators){
        if(str.contains(op)){
            wrap = true;
            break;
        }
    }
    if(wrap){
        return "(" + str + ")";
    }
    return str;
}

void Setter::updateLabel(){
    QString new_label = "???";
    if(lhs_ && operator_ && rhs_){
        auto lhs_value = GetWrappedString(lhs_->getDataValue("value").toString());
        auto operator_value = operator_->getDataValue("label").toString();
        auto rhs_value = GetWrappedString(rhs_->getDataValue("value").toString());

        if(lhs_value.length() && rhs_value.length()){
            new_label = lhs_value + " " + operator_value + " " + rhs_value;
        }
    }
    setDataValue("label", new_label);
}

DataNode* Setter::GetLhs(){
    return lhs_;
}

DataNode* Setter::GetRhs(){
    return rhs_;
}

DataNode* Setter::GetOperator(){
    return operator_;
}