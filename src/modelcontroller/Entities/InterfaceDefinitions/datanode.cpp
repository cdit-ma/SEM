#include "datanode.h"
#include "vectorinstance.h"
#include <QDebug>
#include "../edge.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"


DataNode::DataNode(EntityFactoryBroker& broker, NODE_KIND node_kind, bool is_temp, bool use_complex_types) : Node(broker, node_kind, is_temp){
    setNodeType(NODE_TYPE::DATA);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    if(use_complex_types){
        broker.AttachData(this, "inner_type", QVariant::String, ProtectedState::PROTECTED);
        broker.AttachData(this, "outer_type", QVariant::String, ProtectedState::PROTECTED);
    }
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    
};

bool DataNode::hasInputData()
{
   return getInputData();
}

DataNode *DataNode::getInputData()
{
    for(auto edge : getEdgesOfKind(EDGE_KIND::DATA)){
        if(edge->getDestination() == this){
            return (DataNode*) edge->getSource();
        }
    }
    return 0;
}


void DataNode::setMultipleDataProducer(bool producer)
{
    is_multiple_data_producer_ = producer;
    if(producer){
        setDataProducer(true);
    }
}

void DataNode::setDataProducer(bool producer)
{
    is_producer_ = producer;
    setAcceptsEdgeKind(EDGE_KIND::DATA, EDGE_DIRECTION::SOURCE, is_producer_);
}

void DataNode::setDataReceiver(bool receiver)
{
    is_receiver_ = receiver;
    setAcceptsEdgeKind(EDGE_KIND::DATA, EDGE_DIRECTION::TARGET, is_receiver_);

    if(receiver){
        getFactoryBroker().AttachData(this, "value", QVariant::String, ProtectedState::UNPROTECTED);
    }else{
        getFactoryBroker().RemoveData(this, "value");
    }
}

bool DataNode::isDataProducer() const
{
    return is_producer_;
}

bool DataNode::isDataReceiver() const
{
    return is_receiver_;
}

bool DataNode::isMultipleDataReceiver() const
{
    return is_multiple_data_receiver_;
}

bool DataNode::isMultipleDataProducer() const
{
    return is_multiple_data_producer_;
}


bool DataNode::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }
    
    switch(edge_kind){
    case EDGE_KIND::DATA:{
        if(dst->isNodeOfType(NODE_TYPE::DATA) == false){
            //Cannot connect to a non DataNode type.
            return false;
        }

        DataNode* data_node = (DataNode*) dst;

        if(isDataProducer() == false){
            //Cannot connect from something which can't produce
            return false;
        }

        if(data_node->isDataReceiver() == false){
            //Cannot connect to something which can't recieve
            return false;
        }

        if(data_node->hasInputData() && data_node->isMultipleDataReceiver() != true){
            //Cannot have multiple input datas.
            return false;
        }

        if(isContainedInVector() || data_node->isContainedInVector()){
            //Cannot Data-Connect inside a vector.
            return false;
        }

        if(isContainedInVariable() && getNodeKind() == NODE_KIND::VECTOR){
            //Cannot data-link from a vector itself
            return false;
        }

        if(data_node->isContainedInVariable()){
            //Cannot data link into a variable
            return false;
        }


        if(!isPromiscuousDataLinker() && !data_node->isPromiscuousDataLinker()){
            auto source_containment_node = getContainmentNode();
            auto destination_containment_node = data_node->getContainmentNode();
            
            if(source_containment_node && destination_containment_node){
                auto source_contains_destination = source_containment_node->isAncestorOf(destination_containment_node);

                if(source_contains_destination){
                    //Check to see if we are trying to link into parameters, if the variables scoped parent is the same as the parameter,
                    //We shouldn't allow
                    if(data_node->isNodeOfType(NODE_TYPE::PARAMETER)){
                        if(data_node->getParentNode() == source_containment_node){
                            return false;
                        }
                    }
                }else{
                    //Source needs to contain the destination for scoping to work
                    return false;
                }

                auto src_child_of_containment_node = getChildOfContainmentNode();
                auto dst_child_of_containment_node = data_node->getChildOfContainmentNode();
                
                //Cannot inside the same workflow item
                if(src_child_of_containment_node == dst_child_of_containment_node){
                    return false;
                }
            }else{
                auto source_parent = getParentNode();

                if(!source_parent || !source_parent->isAncestorOf(data_node)){
                    return false;
                }
            }
        }
        
        
        

        if(TypeKey::CompareTypes(this, data_node) == false){
            //Must have compareable types
            return false;
        }
        
        break;
        }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}

bool DataNode::isContainedInVector(){
    RunContainmentChecks();
    return _contained_in_vector;
}

bool DataNode::isContainedInVariable(){
    RunContainmentChecks();
    return _contained_in_variable;
}


void DataNode::RunContainmentChecks(){
    if(getParentNode() && !_run_containment_checks){
        auto parent_nodes = getParentNodes(-1);
        //parent_nodes.push_front(this);
        //Check if we are inside a vector

        Node* previous_node = this;
        for(auto parent_node : parent_nodes){
            if(!_containment_node){
                if(parent_node->isNodeOfType(NODE_TYPE::BEHAVIOUR_CONTAINER)){
                    _containment_node = parent_node;
                    _child_of_containment_node = previous_node;
                }
            }

            switch(parent_node->getNodeKind()){
                case NODE_KIND::VARIABLE:{
                    _contained_in_variable = true;
                    break;
                }
                case NODE_KIND::FUNCTION_CALL:{
                    _contained_in_function_call = true;
                    break;
                }
                case NODE_KIND::AGGREGATE_INSTANCE:{
                    _contained_in_aggregate_instance = true;
                    break;
                }
                
                
                case NODE_KIND::VECTOR:
                case NODE_KIND::VECTOR_INSTANCE:{
                    _contained_in_vector = true;
                    break;
                }
                default:
                    break;
            }
            previous_node = parent_node;
        }
        _run_containment_checks = true;
    }
}

Node* DataNode::getContainmentNode(){
    RunContainmentChecks();
    return _containment_node;
}

Node* DataNode::getChildOfContainmentNode(){
    RunContainmentChecks();
    return _child_of_containment_node;
}

void DataNode::setPromiscuousDataLinker(bool set){
    promiscuous_data_linker_ = set;
}

bool DataNode::isPromiscuousDataLinker() const{
    return promiscuous_data_linker_;
}


void DataNode::BindDataRelationship(Node* source, Node* destination, bool setup){
    if(source && destination && source->isNodeOfType(NODE_TYPE::DATA) && destination->isNodeOfType(NODE_TYPE::DATA)){
        auto source_parent = source->getParentNode();
        auto destination_parent = destination->getParentNode();

        auto is_generic_provider = destination->getDataValue(KeyName::IsGenericParamSrc).toBool();

        //Try and do special data linking
        if(is_generic_provider && destination_parent){
            bool bind_inner_type = true;
            bool bind_outer_type = false;
            QList<Node*> children_to_bind;

            switch(destination_parent->getNodeKind()){
                case NODE_KIND::SETTER:
                case NODE_KIND::BOOLEAN_EXPRESSION:{
                    children_to_bind = destination_parent->getChildren();
                    bind_outer_type = true;
                    break;
                }
                default:{
                    auto destination_second_parent = destination_parent->getParentNode();

                    if(destination_second_parent && destination_second_parent->getNodeKind() == NODE_KIND::FUNCTION_CALL){
                        const auto& class_name = destination_second_parent->getDataValue(KeyName::Class).toString();
                        
                        //Workers OpenCL_Worker and Vector_Operations need special data linking
                        if(class_name == "OpenCL_Worker" || class_name == "Vector_Operations"){
                            for(auto child : destination_second_parent->getChildren()){
                                children_to_bind += child->getChildren();
                            }
                        }
                    }
                    break;
                }
            }
            
            
            for(auto parameter : children_to_bind){
                if(parameter->getDataValue(KeyName::IsGenericParam).toBool()){
                    TypeKey::BindTypes(source, parameter, setup);
                }
            }
        }

        auto bind_source = source;

        if(source_parent && source_parent->getNodeKind() == NODE_KIND::VARIABLE){
            bind_source = source_parent;
        }

        Data::LinkData(bind_source, KeyName::Label, destination, KeyName::Value, setup);

        if(destination->getNodeKind() == NODE_KIND::VARIABLE_PARAMETER){
            TypeKey::BindTypes(source, destination, setup);
        }
    }
}
