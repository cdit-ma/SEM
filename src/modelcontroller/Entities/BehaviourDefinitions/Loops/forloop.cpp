#include "forloop.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../InterfaceDefinitions/datanode.h"

const NODE_KIND node_kind = NODE_KIND::FOR_LOOP;
const QString kind_string = "ForLoop";

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
    broker.AttachData(this, "label", QVariant::String, "for", true);
    broker.ProtectData(this, "index", false);

    //Attach Children
    
    variable_ = broker.ConstructChildNode(*this, NODE_KIND::VARIABLE_PARAMETER);
    auto expression = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::BOOLEAN_EXPRESSION);
    expression_ = expression;
    iteration_ = broker.ConstructChildNode(*this, NODE_KIND::SETTER);

    //Set that the Expression
    expression->setDataReceiver(true);
    expression->setDataProducer(false);


    broker.AttachData(variable_, "label", QVariant::String, "i", false);
    broker.AttachData(variable_, "value", QVariant::Int, 0, false);

    broker.AttachData(iteration_, "icon", QVariant::String, "reload", true);
    broker.AttachData(iteration_, "icon_prefix", QVariant::String, "Icons", true);

    for(auto child : {variable_, expression_, iteration_}){
        broker.AttachData(child, "row", QVariant::Int, 0, true);
        broker.AttachData(child, "column", QVariant::Int, -1, true);
        broker.ProtectData(child, "index", true);
    }

    //Bind Value changing
    auto data_variable_label = variable_->getData("label");
    auto data_variable_value = variable_->getData("value");
    auto data_expression_label = expression_->getData("label");
    auto data_iteration_label = iteration_->getData("label");

    //Update Label on data Change
    connect(data_variable_label, &Data::dataChanged, this, &MEDEA::ForLoop::updateLabel);
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
        auto var_label = variable_->getDataValue("label").toString();
        auto var_value = variable_->getDataValue("value").toString();
        
        auto expression_label = expression_->getDataValue("label").toString();
        auto itterator_label = iteration_->getDataValue("label").toString();

        new_label += var_label;
        new_label += "=" + var_value + "; ";
        new_label += expression_label + "; ";
        new_label += itterator_label + "";
        new_label += ")";
    }
    setDataValue("label", new_label);
}