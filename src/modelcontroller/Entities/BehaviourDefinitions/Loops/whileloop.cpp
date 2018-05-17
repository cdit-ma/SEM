#include "whileloop.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../InterfaceDefinitions/datanode.h"

const NODE_KIND node_kind = NODE_KIND::WHILE_LOOP;
const QString kind_string = "While Loop";

void MEDEA::WhileLoop::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::WhileLoop(broker, is_temp_node);
        });
}

MEDEA::WhileLoop::WhileLoop(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);

    setAcceptsNodeKind(NODE_KIND::VARIABLE_PARAMETER);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);

    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    setLabelFunctional(false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "label", QVariant::String, "while", true);
    broker.ProtectData(this, "index", false);
    
    auto expression = (DataNode*) broker.ConstructChildNode(*this, NODE_KIND::BOOLEAN_EXPRESSION);
    expression_ = expression;
    broker.AttachData(expression, "row", QVariant::Int, 0, true);
    broker.AttachData(expression, "column", QVariant::Int, -1, true);
    broker.ProtectData(expression, "index", true);

    //Set that the Expression can accept data
    expression->setDataReceiver(true);
    expression->setDataProducer(false);

    auto data_expression_label = expression_->getData("label");
    connect(data_expression_label, &Data::dataChanged, this, &MEDEA::WhileLoop::updateLabel);
    updateLabel();
}


void MEDEA::WhileLoop::updateLabel(){
    QString new_label = "while";
    if(expression_){
        new_label += "(";
        auto expression_label = expression_->getDataValue("label").toString();
        new_label += expression_label;
        new_label += ")";
    }
    setDataValue("label", new_label);
}