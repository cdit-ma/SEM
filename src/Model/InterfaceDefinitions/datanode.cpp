#include "datanode.h"
#include "vectorinstance.h"
#include <QDebug>
//#include "../entityfactory.h"

DataNode::DataNode(EntityFactory* factory, NODE_KIND kind, QString kind_str) : Node(factory, kind, kind_str){
};

DataNode::DataNode(NODE_KIND kind):Node(kind)
{
    setAcceptsEdgeKind(EDGE_KIND::DATA);
    setNodeType(NODE_TYPE::DATA);
    _isProducer = false;
    _isReciever = false;
}

bool DataNode::hasInputData()
{
   return getInputData();
}

bool DataNode::hasOutputData()
{
    return getOutputData();
}

DataNode *DataNode::getInputData()
{
    foreach(Edge* edge, getEdges(0, EDGE_KIND::DATA)){
        if(edge->getDestination() == this){
            return (DataNode*) edge->getSource();
        }
    }
    return 0;
}

DataNode *DataNode::getOutputData()
{
    foreach(Edge* edge, getEdges(0, EDGE_KIND::DATA)){
        if(edge->getSource() == this){
            return (DataNode*) edge->getDestination();
        }
    }
    return 0;
}

void DataNode::setMultipleDataReceiver(bool receiver)
{
    _isMultipleDataReceiver = receiver;
}

void DataNode::setDataProducer(bool producer)
{
    _isProducer = producer;
}

void DataNode::setDataReciever(bool reciever)
{
    _isReciever = reciever;
}

bool DataNode::isDataProducer() const
{
    return _isProducer;
}

bool DataNode::isDataReciever() const
{
    return _isReciever;
}

bool DataNode::isMultipleDataReceiver() const
{
    return _isMultipleDataReceiver;
}

bool DataNode::comparableTypes(DataNode *node)
{
    QStringList numberTypes;
    numberTypes << "Float" << "Double" << "Integer" << "Boolean";

    if(node){
        if(node->getNodeKind() == NODE_KIND::VARIADIC_PARAMETER){
           return true;
        }

        //Types
        QString type1 = getDataValue("type").toString();
        QString type2 = node->getDataValue("type").toString();

        if(type1 == type2 && type1 != ""){
            //Allow direct matches.
            return true;
        }

        if(numberTypes.contains(type1) && numberTypes.contains(type2)){
            //Allow matches of numbers
            return true;
        }


        if(type2 == "Vector"){
            auto kind = getNodeKind();
            if(kind == NODE_KIND::VECTOR || kind == NODE_KIND::VECTOR_INSTANCE){
               return true;
            }
        }
        if(type1 == "Vector"){
            auto kind = node->getNodeKind();
            if(kind == NODE_KIND::VECTOR || kind == NODE_KIND::VECTOR_INSTANCE){
               return true;
            }
        }
        if(type2 == ""){
            return true;
        }
    }
    return false;

}

bool DataNode::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case EDGE_KIND::DATA:{
        if(!dst->isNodeOfType(NODE_TYPE::DATA)){
            //Cannot connect to a non DataNode type.
            return false;
        }
        DataNode* dataNode = (DataNode*) dst;

        if(!isDataProducer()){
            //qCritical() << "Cannot connect from something which can't produce";
            //Cannot connect from something which can't produce
            return false;
        }
        if(!dataNode->isDataReciever()){
            //qCritical() << "Cannot connect to something which can't recieve";
            //Cannot connect to something which can't recieve
            return false;
        }

        if(!dataNode->isMultipleDataReceiver() && dataNode->hasInputData()){
            //qCritical() << "Cannot have multiple input datas";
            //Cannot have multiple input datas.
            return false;
        }







        int heightToAncestor = getDepthFromCommonAncestor(dst);
        int heightToComponentImpl = getDepthFromAspect() - 1;

        if(heightToAncestor > heightToComponentImpl){
            //Cannot connect to something outside of the same Component.
            return false;
        }

        Node* sharedAncestor = getParentNode(heightToAncestor);
        if(sharedAncestor){
            if(sharedAncestor->getNodeKind() == NODE_KIND::AGGREGATE_INSTANCE){
                //Can't data connect if our shared parent is an Aggregate Instance.
                return false;
            }
        }

        //Compare types!
        if(!comparableTypes(dataNode)){
            return false;
        }
        break;
        }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}

